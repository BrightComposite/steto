#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QQmlListProperty>

#include <Device>
#include <RecordingService>

#include <QBluetoothUuid>
#include <Async>

class BtConnection;
class QBluetoothLocalDevice;
class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyController;

class DeviceService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Device * currentDevice READ currentDevice NOTIFY currentDeviceChanged)
	Q_PROPERTY(QQmlListProperty<Device> devices READ devices NOTIFY devicesChanged)
	Q_PROPERTY(bool isValid READ isValid CONSTANT)
	Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
	Q_PROPERTY(bool isConnecting READ isConnecting NOTIFY connectingChanged)
	Q_PROPERTY(RecordingService * recording READ recording CONSTANT)

public:
	explicit DeviceService(QObject *parent = 0);

	Device * currentDevice() const;
	QQmlListProperty<Device> devices();
	bool isValid() const;
	bool isConnected() const;
	bool isConnecting() const;
	RecordingService * recording() const;

	static QBluetoothUuid IO_SERVICE;
	static QBluetoothUuid IO_CHARACTERISTIC;

signals:
	void currentDeviceChanged();
	void devicesChanged();
	void connectedChanged();
	void connectingChanged();

	void enabled();
	void disabled();
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
	QFuture<void> enable();
	QFuture<void> discoverDevices();
	QFuture<void> findDevice(const QString & address);
	QFuture<void> connectToDevice();
	QFuture<void> disconnectFromDevice();
	QFuture<void> discoverServices();
	QFuture<void> connectToService();
	QFuture<void> subscribeToService();

	void clearDevices();
	void clearServices();

	Device * _currentDevice = nullptr;
	QList<Device *> _devices;

	BtConnection * _connection;
	RecordingService * _recording;
	bool _isConnecting = false;

	QBluetoothLocalDevice * _local = nullptr;
	QBluetoothDeviceDiscoveryAgent * _agent = nullptr;
	QLowEnergyController * _le = nullptr;

	std::vector<QBluetoothUuid> _availableServices;
};

#endif // DEVICESERVICE_H
