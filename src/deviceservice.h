#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QQmlListProperty>

#include <Device>
#include <DataService>

#include <QBluetoothUuid>
#include <taskqueue.h>

class BtConnection;
class QBluetoothLocalDevice;
class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyController;

class DeviceService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Device * currentDevice READ currentDevice NOTIFY currentDeviceChanged)
	Q_PROPERTY(QQmlListProperty<Device> devices READ devices NOTIFY devicesChanged)
	Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
	Q_PROPERTY(DataService * data READ data CONSTANT)

public:
	explicit DeviceService(QObject *parent = 0);

	Device * currentDevice() const;
	QQmlListProperty<Device> devices();
	bool isConnected() const;
	DataService * data() const;

	static QBluetoothUuid IO_SERVICE;
	static QBluetoothUuid IO_CHARACTERISTIC;

signals:
	void currentDeviceChanged();
	void devicesChanged();
	void connectedChanged();

	void enabled();
	void connected();
	void disconnected();
	void devicesFound();
	void deviceDiscoveryCancel();
	void servicesFound();
	void serviceDiscoveryCancel();

public slots:
	void search();
	void select(const QString & address);

private:
	Deferred<void> findDevice();
	Deferred<void> enable();

	void clearDevices();
	void clearServices();

	Device * _currentDevice = nullptr;
	QList<Device *> _devices;

	BtConnection * _connection;
	DataService * _data;

	QBluetoothLocalDevice * _local = nullptr;
	QBluetoothDeviceDiscoveryAgent * _agent = nullptr;
	QLowEnergyController * _le = nullptr;

	std::vector<QBluetoothUuid> _availableServices;
};

#endif // DEVICESERVICE_H
