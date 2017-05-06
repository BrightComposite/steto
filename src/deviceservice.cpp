#include "deviceservice.h"

#include <functional>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QBluetoothLocalDevice>

#include <QDebug>

#include <BtConnection>

QBluetoothUuid DeviceService::IO_SERVICE = QBluetoothUuid(QStringLiteral("0000ffe0-0000-1000-8000-00805f9b34fb"));
QBluetoothUuid DeviceService::IO_CHARACTERISTIC = QBluetoothUuid(QStringLiteral("0000ffe1-0000-1000-8000-00805f9b34fb"));

DeviceService::DeviceService(QObject *parent) : QObject(parent) {
	qDebug() << "Create DeviceService...";

	_local = new QBluetoothLocalDevice(this);
	_agent = new QBluetoothDeviceDiscoveryAgent(this);
	_connection = new BtConnection(this);
	_data = new DataService(_connection);

	connect(_local, &QBluetoothLocalDevice::hostModeStateChanged, [this](QBluetoothLocalDevice::HostMode state) {
		if(state == QBluetoothLocalDevice::HostConnectable) {
			qDebug() << "Bluetooth enabled!";
			emit enabled();
		}
	});
/*
	connect(_local, &QBluetoothLocalDevice::pairingFinished, [this](const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
		qDebug() << "Device paired!";
		_tasks->succeeded(Tasks::PAIRING);
	});

	connect(_local, static_cast<void(QBluetoothLocalDevice::*)(QBluetoothLocalDevice::Error)>(&QBluetoothLocalDevice::error), [this](QBluetoothLocalDevice::Error error) {
		qWarning() << error;
		_tasks->failed(Tasks::PAIRING);
	});
*/
	connect(_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, [this](const QBluetoothDeviceInfo & info) {
		if((info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) != 0) {
			qDebug() << "Found LE device: " << info.name() << "(" + info.address().toString() + ")";
			_devices.append(new Device(info));
			emit devicesChanged();
		}
	});

	connect(_agent, &QBluetoothDeviceDiscoveryAgent::finished, [this]() {
		qDebug() << "Device discovery finished!";
		emit devicesFound();
	});

	connect(_agent, static_cast<void(QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error), [this](QBluetoothDeviceDiscoveryAgent::Error error) {
		qWarning() << error;
		emit deviceDiscoveryCancel();
	});

	connect(_connection, &BtConnection::connected, [this]() {
		emit connectedChanged();
		emit connected();
	});

	connect(_connection, &BtConnection::disconnected, [this]() {
		emit connectedChanged();
		emit disconnected();
	});
}

Device * DeviceService::currentDevice() const {
	return _currentDevice;
}

QQmlListProperty<Device> DeviceService::devices() {
	return {this, _devices};
}

bool DeviceService::isConnected() const {
	return _connection->service() != nullptr;
}

DataService * DeviceService::data() const {
	return _data;
}

void DeviceService::clearDevices() {
	clearServices();

	_currentDevice = nullptr;
	auto devices = _devices;
	_devices.clear();
	emit devicesChanged();
	qDeleteAll(devices);
}

void DeviceService::clearServices() {
	_connection->select(nullptr);
	_availableServices.clear();

	if(_le) {
		_le->deleteLater();
		_le = nullptr;
	}
}

void DeviceService::search() {
	qDebug() << "Start device discovery...";

	_agent->stop();
	emit deviceDiscoveryCancel();

	clearDevices();

	TaskQueue::future(this, &DeviceService::enable)
	->then([this]() {
		auto d = async::subscribe(
			async::observe(this, &DeviceService::devicesFound),
			async::observe(this, &DeviceService::deviceDiscoveryCancel)
		);

		_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

		return d;
	})->run();
}

void DeviceService::select(const QString & address) {
	if(_currentDevice && _currentDevice->address() == address) {
		qDebug() << "Device" << _currentDevice->name() << "(" + _currentDevice->address() + ")" << "is already connected";
		return;
	}

	_agent->stop();
	emit devicesFound();

	auto b = TaskQueue::future([this]() {
		if(!_le) {
			return async::complete();
		}

		if(_currentDevice) {
			auto d = async::subscribe(async::observe(this, &DeviceService::disconnected));
			_le->disconnectFromDevice();
			return d;
		}

		clearServices();
		return async::complete();
	});

	if(address == "0") {
		return b->run();
	}

	b->then([this](const QString & address) {
		auto i = std::find_if(_devices.begin(), _devices.end(), [address](Device * d) {
			return d->address() == address;
		});

		if(i == _devices.end()) {
			qWarning() << "Can't find device with address" << address;
			return async::cancel();
		}

		_currentDevice = *i;
		emit currentDeviceChanged();

		emit serviceDiscoveryCancel();

		_le = new QLowEnergyController(_currentDevice->info(), this);

		connect(_le, &QLowEnergyController::connected, [this]() {
			qDebug() << "Connected to" << _currentDevice->name();
			emit connected();
		});

		connect(_le, &QLowEnergyController::disconnected, [this]() {
			qDebug() << "Disconnected from" << (_currentDevice ? _currentDevice->name() : "null");
			_currentDevice = nullptr;
			emit currentDeviceChanged();

			clearServices();

			emit serviceDiscoveryCancel();
			emit disconnected();
		});

		connect(_le, &QLowEnergyController::serviceDiscovered, [this](const QBluetoothUuid & uuid) {
			qDebug() << "Found service:" << uuid.toString();
			_availableServices.push_back(uuid);
		});

		connect(_le, &QLowEnergyController::discoveryFinished, [this]() {
			qDebug() << "Service discovery finished!";
			emit servicesFound();
		});

		connect(_le, static_cast<void(QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), [this](QLowEnergyController::Error error) {
			qWarning() << error;
			emit serviceDiscoveryCancel();
		});

		return async::complete();
	}, address)->then([this]() {
		qDebug() << "Connect to" << _currentDevice->name();

		auto d = async::subscribe(
			async::observe(this, &DeviceService::connected),
			async::observe(this, &DeviceService::disconnected)
		);

		_le->connectToDevice();

		return d;
	})->then([this]() {
		qDebug() << "Start service discovery for" << _currentDevice->name();

		auto d = async::subscribe(
			async::observe(this, &DeviceService::servicesFound),
			async::observe(this, &DeviceService::serviceDiscoveryCancel)
		);

		_le->discoverServices();

		return d;
	})->then([this]() {
		if(std::find(_availableServices.begin(), _availableServices.end(), DeviceService::IO_SERVICE) == _availableServices.end()) {
			qWarning() << "IO service not found!";
			return async::cancel();
		}

		qDebug() << "Connect to service" << DeviceService::IO_SERVICE;
		return _connection->select(_le->createServiceObject(DeviceService::IO_SERVICE, _le));
	})->then([this]() {
		auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
		_connection->subscribe(c);

		return async::complete();
	})->run();
}

Deferred<void> DeviceService::findDevice() {

}

Deferred<void> DeviceService::enable() {
	if(_local->hostMode() == QBluetoothLocalDevice::HostConnectable) {
		return async::complete();
	}

	qDebug() << "Enable bluetooth...";

	auto d = async::subscribe(async::observe(this, &DeviceService::enabled));
	_local->setHostMode(QBluetoothLocalDevice::HostConnectable);

	return d;
}
