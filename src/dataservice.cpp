#include "dataservice.h"
#include <DeviceService>
#include <QDebug>

DataService::DataService(BtConnection * connection) : QObject(connection), _connection(connection) {
	_active = _connection->service() != nullptr;

	connect(_connection, &BtConnection::disconnected, [this]() {
		if(_active) {
			_active = false;
			emit activeChanged();
		}
	});
}

bool DataService::active() const {
	return _active;
}

void DataService::toggle() {
	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return;
	}

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	_connection->write(c, _active ? "A0" : "A1");

	_active = !_active;
	emit activeChanged();
}

void DataService::activate() {
	if(_active) {
		qDebug() << "Already activated";
		return;
	}

	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return;
	}

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	_connection->write(c, "A1");

	_active = true;
	emit activeChanged();
}

void DataService::deactivate() {
	if(!_active) {
		qDebug() << "Already inactive";
		return;
	}

	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return;
	}

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	_connection->write(c, "A0");

	_active = false;
	emit activeChanged();
}
