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

	_connection = new BtConnection(this);
	_recording = new RecordingService(_connection);

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	_local = new QBluetoothLocalDevice(this);

	connect(_local, &QBluetoothLocalDevice::hostModeStateChanged, [this](QBluetoothLocalDevice::HostMode state) {
		if(state == QBluetoothLocalDevice::HostConnectable) {
			qDebug() << "Bluetooth enabled!";
			emit enabled();
		} if(state == QBluetoothLocalDevice::HostPoweredOff) {
			qDebug() << "Bluetooth disabled!";
			clearDevices();
			emit disabled();
		} else {
			qDebug() << "Device state changed:" << state;
		}
	});

	if(_local->isValid()) {
		_agent = new QBluetoothDeviceDiscoveryAgent(this);

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

		connect(_agent, &QBluetoothDeviceDiscoveryAgent::canceled, [this]() {
			qDebug() << "Device discovery canceled!";
		});

		connect(_agent, static_cast<void(QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error), [this](QBluetoothDeviceDiscoveryAgent::Error error) {
			qWarning() << error;
			emit deviceDiscoveryCancel();
		});
	}

	connect(_connection, &BtConnection::connected, [this]() {
		emit connectedChanged();
		emit connected();
	});

	connect(_connection, &BtConnection::disconnected, [this]() {
		emit connectedChanged();
		emit disconnected();
	});
#endif
}

Device * DeviceService::currentDevice() const {
	return _currentDevice;
}

QQmlListProperty<Device> DeviceService::devices() {
	return {this, _devices};
}

bool DeviceService::isValid() const {
	return _local && _local->isValid();
}

bool DeviceService::isConnected() const {
	return _connection->service() != nullptr;
}

bool DeviceService::isConnecting() const {
	return _isConnecting;
}

RecordingService * DeviceService::recording() const {
	return _recording;
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
	if(_isConnecting) {
		_isConnecting = false;
		emit connectingChanged();
	}

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

	async::chain(this, &DeviceService::enable)
	-> then(this, &DeviceService::discoverDevices)
	-> cancel_on(this, &DeviceService::disabled)
	-> run();
}

void DeviceService::select(const QString & address) {
	if(_isConnecting) {
		qDebug() << "Already connecting";
		return;
	}

	if(_currentDevice && _currentDevice->address() == address) {
		qDebug() << "Device" << _currentDevice->name() << "(" + _currentDevice->address() + ")" << "is already connected";
		return;
	}

	_agent->stop();
	emit devicesFound();

	auto task = _le != nullptr ? async::chain(this, &DeviceService::disconnectFromDevice) : async::chain();

	if(address == "0") {
		_currentDevice = nullptr;
		emit currentDeviceChanged();
		return task->run();
	}

	_isConnecting = true;
	emit connectingChanged();

	task
	-> then(this, &DeviceService::findDevice, address)
	-> then(this, &DeviceService::connectToDevice)
	-> then(this, &DeviceService::discoverServices)
	-> then(this, &DeviceService::connectToService)
	-> then(this, &DeviceService::subscribeToService)

	-> cancel_on(this, &DeviceService::disabled)

	-> end_with([this]() {
		_isConnecting = false;
		emit connectingChanged();
	})

	-> run();
}

QFuture<void> DeviceService::enable() {
	if(_local->hostMode() == QBluetoothLocalDevice::HostConnectable) {
		return async::complete();
	}

	qDebug() << "Enable bluetooth...";

	auto f = async::connect(async::observe(this, &DeviceService::enabled));

	_local->setHostMode(QBluetoothLocalDevice::HostConnectable);
	return f;
}

QFuture<void> DeviceService::discoverDevices() {
	auto f = async::connect(
		async::observe(this, &DeviceService::devicesFound),
		async::observe(this, &DeviceService::deviceDiscoveryCancel)
	);

	_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

	return f;
}

QFuture<void> DeviceService::findDevice(const QString & address) {
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
		if(_currentDevice) {
			qDebug() << "Connected to" << _currentDevice->name();
			emit connected();
		}
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

	connect(_le, &QLowEnergyController::stateChanged, [this](QLowEnergyController::ControllerState state) {
		qDebug() << "Controller state changed:" << state;
	});

	connect(_le, static_cast<void(QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), [this](QLowEnergyController::Error error) {
		qWarning() << error;
		emit serviceDiscoveryCancel();
	});

	return async::complete();
}

QFuture<void> DeviceService::connectToDevice() {
	if(!_currentDevice) {
		return async::cancel();
	}

	qDebug() << "Connect to" << _currentDevice->name();

	auto f = async::connect(
		async::observe(this, &DeviceService::connected),
		async::observe(this, &DeviceService::disconnected)
	);

	_le->connectToDevice();

	return f;
}

QFuture<void> DeviceService::disconnectFromDevice() {
	if(!_le) {
		_currentDevice = nullptr;
		return async::complete();
	}

	if(_currentDevice) {
		if(_le->state() != QLowEnergyController::UnconnectedState) {
			auto f = async::connect(async::observe(this, &DeviceService::disconnected));
			_le->disconnectFromDevice();
			return f;
		}

		_currentDevice = nullptr;
	}

	clearServices();
	return async::complete();
}

QFuture<void> DeviceService::discoverServices() {
	if(!_currentDevice) {
		qWarning() << "Can't discover services: there is no device selected";
		return async::cancel();
	}

	qDebug() << "Start service discovery for" << _currentDevice->name();

	auto f = async::connect(
		async::observe(this, &DeviceService::servicesFound),
		async::observe(this, &DeviceService::serviceDiscoveryCancel)
	);

	_le->discoverServices();

	return f;
}

QFuture<void> DeviceService::connectToService() {
	if(!_le) {
		return async::cancel();
	}

	if(std::find(_availableServices.begin(), _availableServices.end(), DeviceService::IO_SERVICE) == _availableServices.end()) {
		qWarning() << "IO service not found!";
		return async::cancel();
	}

	qDebug() << "Connect to service" << DeviceService::IO_SERVICE;
	return _connection->select(_le->createServiceObject(DeviceService::IO_SERVICE, _le));
}

QFuture<void> DeviceService::subscribeToService() {
	if(!_connection->service()) {
		return async::cancel();
	}

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	_connection->subscribe(c);

	return async::complete();
}
