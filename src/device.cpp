#include "device.h"
#include <QBluetoothAddress>

Device::Device() {

}

Device::Device(const QBluetoothDeviceInfo & info) : _info(info) {

}

const QBluetoothDeviceInfo & Device::info() const {
	return _info;
}

QString Device::name() const {
	return _info.name();
}

QString Device::address() const {
#ifdef Q_OS_MAC
	return _info.deviceUuid().toString();
#else
	return _info.address().toString();
#endif
}

void Device::setInfo(const QBluetoothDeviceInfo & info) {
	_info = info;
	emit infoChanged();
}
