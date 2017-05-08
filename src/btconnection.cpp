#include "btconnection.h"

#include <QLowEnergyService>

BtConnection::BtConnection(QObject *parent) : QObject(parent)
{

}

QLowEnergyService * BtConnection::service() const {
	return _service;
}

QFuture<void> BtConnection::select(QLowEnergyService * service) {
	if(_service) {
		delete _service;
		_service = nullptr;
		emit disconnected();
	}

	_service = service;

	if(!_service) {
		return async::complete();
	}

	connect(_service, &QLowEnergyService::stateChanged, [this](const QLowEnergyService::ServiceState state) {
		qDebug() << "Service state" << state;

		if(state == QLowEnergyService::ServiceDiscovered) {
			emit connected();
		}
	});

	connect(_service, &QLowEnergyService::characteristicWritten, [this](const QLowEnergyCharacteristic & c, const QByteArray & value) {
		qDebug() << "Characteristic written" << c.uuid() << "value:" << value;
		emit written();

		for(auto & d : _writes[c.uuid().toString()]) {
			d.complete();
		}
	});

	connect(_service, &QLowEnergyService::characteristicChanged, [this](const QLowEnergyCharacteristic & c, const QByteArray & value) {
		for(auto & cb : _callbacks[c.uuid().toString()]) {
			cb(value);
		}

		emit changed();
	});

	connect(_service, &QLowEnergyService::characteristicRead, [this](const QLowEnergyCharacteristic & c, const QByteArray & value) {
		qDebug() << "Characteristic read" << c.uuid() << "value:" << value;
		emit read();

		for(auto & d : _reads[c.uuid().toString()]) {
			d.complete();
		}
	});

	connect(_service, static_cast<void(QLowEnergyService::*)(QLowEnergyService::ServiceError)>(&QLowEnergyService::error), [this](QLowEnergyService::ServiceError error) {
		qWarning() << error;

		if(_service) {
			delete _service;
			_service = nullptr;
			emit disconnected();
		}
	});

	auto f = async::connect(
		async::observe(this, &BtConnection::connected),
		async::observe(this, &BtConnection::disconnected)
	);

	_service->discoverDetails();

	return f;
}

QLowEnergyCharacteristic BtConnection::characteristic(const QBluetoothUuid & uuid) {
	return _service->characteristic(uuid);
}

QFuture<void> BtConnection::write(const QLowEnergyCharacteristic & c, const QByteArray & data) {
	if(!_service) {
		qWarning() << "Can't write to characteristic" << c.uuid() << "- there is no active service";
		return async::cancel();
	}

	auto d = async::deferred<void>();
	_writes[c.uuid().toString()].push_back(d);

	_service->writeCharacteristic(c, data);

	return async::connect(
		d.future(),
		async::observe(this, &BtConnection::disconnected)
	);
}

QFuture<void> BtConnection::read(const QLowEnergyCharacteristic & c) {
	if(!_service) {
		qWarning() << "Can't read from characteristic" << c.uuid() << "- there is no active service";
		return async::cancel();
	}

	auto d = async::deferred<void>();
	_reads[c.uuid().toString()].push_back(d);

	_service->readCharacteristic(c);

	return async::connect(
		d.future(),
		async::observe(this, &BtConnection::disconnected)
	);
}

void BtConnection::subscribe(const QLowEnergyCharacteristic & c) {
	if(!_service) {
		qWarning() << "Can't subscribe to characteristic" << c.uuid() << "- there is no active service";
		return;
	}

	auto desc = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

	if (desc.isValid()) {
		_service->writeDescriptor(desc, QByteArray::fromHex("0100"));
	}
}

void BtConnection::notify(const QBluetoothUuid & uuid, const std::function<void(const QByteArray &)> & cb) {
	_callbacks[uuid.toString()].push_back(cb);
}

