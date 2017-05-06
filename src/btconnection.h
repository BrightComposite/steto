#ifndef BTCONNECTION_H
#define BTCONNECTION_H

#include <QObject>
#include <QBluetoothUuid>
#include <QLowEnergyCharacteristic>

#include <Task>

class QLowEnergyService;

class BtConnection : public QObject
{
	Q_OBJECT
public:
	explicit BtConnection(QObject *parent = 0);

	QLowEnergyService * service() const;
	void select(QLowEnergyService * service, Task * t = nullptr);

	QLowEnergyCharacteristic characteristic(const QBluetoothUuid & uuid);
	void write(const QLowEnergyCharacteristic & c, const QByteArray & data);
	void read(const QLowEnergyCharacteristic & c);
	void subscribe(const QLowEnergyCharacteristic & c);

signals:
	void connected();
	void disconnected();

public slots:

private:
	enum Tasks {
		DISCOVER_DETAILS
	};

	TaskPool _tasks;
	QLowEnergyService * _service = nullptr;
};

#endif // BTCONNECTION_H
