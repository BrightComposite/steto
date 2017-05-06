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
	_tasks = new TaskPool(this);
	_connection = new BtConnection(this);
	_data = new DataService(_connection);

	connect(_local, &QBluetoothLocalDevice::hostModeStateChanged, [this](QBluetoothLocalDevice::HostMode state) {
		if(state == QBluetoothLocalDevice::HostConnectable) {
			qDebug() << "Bluetooth enabled!";
			_tasks->succeeded(Tasks::ENABLE);
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
		_tasks->succeeded(Tasks::SEARCH);
	});

	connect(_agent, static_cast<void(QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error), [this](QBluetoothDeviceDiscoveryAgent::Error error) {
		qWarning() << error;
		_tasks->failed(Tasks::SEARCH);
	});

	connect(_connection, &BtConnection::connected, [this]() {
		emit connectedChanged();
	});

	connect(_connection, &BtConnection::disconnected, [this]() {
		emit connectedChanged();
	});
}

Device * DeviceService::currentDevice() const {
	return _currentDevice;
}

QQmlListProperty<Device> DeviceService::devices() {
	return {this, _devices};
}

bool DeviceService::connected() const {
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
	_tasks->failed(Tasks::SEARCH);

	clearDevices();

	Schedule::task("enable bt", [this](Task * t) {
		enable(t);
	})->then("search devices", [this](Task * t) {
		_tasks->add(Tasks::SEARCH, t);
		_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
	})->run();
}

void DeviceService::select(const QString & address) {
	if(_currentDevice && _currentDevice->address() == address) {
		qDebug() << "Device" << _currentDevice->name() << "(" + _currentDevice->address() + ")" << "is already connected";
		return;
	}

	_agent->stop();
	_tasks->succeeded(Tasks::SEARCH);

	auto * s = Schedule::task("clear existent connections", [this](Task * t) {
		if(!_le) {
			return t->succeed();
		}

		if(_currentDevice) {
			_tasks->add(Tasks::DISCONNECT, t);
			return _le->disconnectFromDevice();
		}

		clearServices();
	});

	if(address == "0") {
		return s->run();
	}

	s->then("find device", [this, address](Task * t) {
		auto i = std::find_if(_devices.begin(), _devices.end(), [address](Device * d) {
			return d->address() == address;
		});

		if(i == _devices.end()) {
			qWarning() << "Can't find device with address" << address;
			return t->fail();
		}

		_currentDevice = *i;
		emit currentDeviceChanged();

		_tasks->failed(Tasks::CONNECT);
		_tasks->failed(Tasks::DISCOVERY);

		_le = new QLowEnergyController(_currentDevice->info(), this);

		connect(_le, &QLowEnergyController::connected, [this]() {
			qDebug() << "Connected to" << _currentDevice->name();
			_tasks->succeeded(Tasks::CONNECT);
		});

		connect(_le, &QLowEnergyController::disconnected, [this]() {
			qDebug() << "Disconnected from" << (_currentDevice ? _currentDevice->name() : "null");
			_currentDevice = nullptr;
			emit currentDeviceChanged();

			clearServices();

			_tasks->failed(Tasks::CONNECT);
			_tasks->failed(Tasks::DISCOVERY);
			_tasks->succeeded(Tasks::DISCONNECT);
		});

		connect(_le, &QLowEnergyController::serviceDiscovered, [this](const QBluetoothUuid & uuid) {
			qDebug() << "Found service:" << uuid.toString();
			_availableServices.push_back(uuid);
		});

		connect(_le, &QLowEnergyController::discoveryFinished, [this]() {
			qDebug() << "Service discovery finished!";
			_tasks->succeeded(Tasks::DISCOVERY);
		});

		connect(_le, static_cast<void(QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), [this](QLowEnergyController::Error error) {
			qWarning() << error;
			_tasks->failed(Tasks::DISCOVERY);
		});

		return t->succeed();
	})->then("connect to device", [this](Task * t) {
		qDebug() << "Connect to" << _currentDevice->name();

		_tasks->add(Tasks::CONNECT, t);
		_le->connectToDevice();
	})->then("discover services", [this](Task * t) {
		qDebug() << "Start service discovery for" << _currentDevice->name();

		_tasks->add(Tasks::DISCOVERY, t);
		_le->discoverServices();
	})->then("connect to service", [this](Task * t) {
		if(std::find(_availableServices.begin(), _availableServices.end(), DeviceService::IO_SERVICE) == _availableServices.end()) {
			qWarning() << "IO service not found!";
			return t->fail();
		}

		qDebug() << "Connect to service" << DeviceService::IO_SERVICE;
		return _connection->select(_le->createServiceObject(DeviceService::IO_SERVICE, _le), t);
	})->then("subscribe to notifications", [this](Task * t) {
		auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
		_connection->subscribe(c);
		return t->succeed();
	})->run();
}

void DeviceService::enable(Task * task) {
	if(_local->hostMode() == QBluetoothLocalDevice::HostConnectable) {
		return task->succeed();
	}

	qDebug() << "Enable bluetooth...";

	_tasks->add(Tasks::ENABLE, task);
	_local->setHostMode(QBluetoothLocalDevice::HostConnectable);
}
