#include "btconnection.h"

#include <QLowEnergyService>

BtConnection::BtConnection(QObject *parent) : QObject(parent)
{

}

QLowEnergyService * BtConnection::service() const {
	return _service;
}

Deferred<void> BtConnection::select(QLowEnergyService * service) {
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

	connect(_service, &QLowEnergyService::characteristicWritten, [](const QLowEnergyCharacteristic & info, const QByteArray & value) {
		qDebug() << "Characteristic written" << info.uuid() << "value:" << value;
	});

	connect(_service, &QLowEnergyService::characteristicChanged, [](const QLowEnergyCharacteristic & info, const QByteArray & value) {
		qDebug() << "Characteristic changed" << info.uuid() << "value:" << value;
	});

	connect(_service, &QLowEnergyService::characteristicRead, [](const QLowEnergyCharacteristic & info, const QByteArray & value) {
		qDebug() << "Characteristic read" << info.uuid() << "value:" << value;
	});

	connect(_service, static_cast<void(QLowEnergyService::*)(QLowEnergyService::ServiceError)>(&QLowEnergyService::error), [this](QLowEnergyService::ServiceError error) {
		qWarning() << error;

		if(_service) {
			delete _service;
			_service = nullptr;
			emit disconnected();
		}
	});

	auto d = async::subscribe(
		async::observe(this, &BtConnection::connected),
		async::observe(this, &BtConnection::disconnected)
	);

	_service->discoverDetails();

	return d;
}

QLowEnergyCharacteristic BtConnection::characteristic(const QBluetoothUuid & uuid) {
	return _service->characteristic(uuid);
}

void BtConnection::write(const QLowEnergyCharacteristic & c, const QByteArray & data) {
	if(!_service) {
		qWarning() << "Can't write to characteristic" << c.uuid() << "- there is no active service";
		return;
	}

	_service->writeCharacteristic(c, data);
}

void BtConnection::read(const QLowEnergyCharacteristic & c) {
	if(!_service) {
		qWarning() << "Can't read from characteristic" << c.uuid() << "- there is no active service";
		return;
	}

	_service->readCharacteristic(c);
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

