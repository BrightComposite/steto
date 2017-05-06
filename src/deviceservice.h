#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QQmlListProperty>

#include <Device>
#include <Task>
#include <DataService>

#include <QBluetoothUuid>

class BtConnection;
class QBluetoothLocalDevice;
class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyController;

class DeviceService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Device * currentDevice READ currentDevice NOTIFY currentDeviceChanged)
	Q_PROPERTY(QQmlListProperty<Device> devices READ devices NOTIFY devicesChanged)
	Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
	Q_PROPERTY(DataService * data READ data CONSTANT)

public:
	explicit DeviceService(QObject *parent = 0);

	Device * currentDevice() const;
	QQmlListProperty<Device> devices();
	bool connected() const;
	DataService * data() const;

	static QBluetoothUuid IO_SERVICE;
	static QBluetoothUuid IO_CHARACTERISTIC;

signals:
	void currentDeviceChanged();
	void devicesChanged();
	void connectedChanged();

public slots:
	void search();
	void select(const QString & address);

private:
	enum Tasks {
		ENABLE,
		SEARCH,
		CONNECT,
		DISCONNECT,
		DISCOVERY
	};

	void enable(Task * task);
	void clearDevices();
	void clearServices();

	TaskPool * _tasks = nullptr;

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
