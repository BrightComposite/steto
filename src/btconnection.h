#ifndef BTCONNECTION_H
#define BTCONNECTION_H

#include <QObject>
#include <QBluetoothUuid>
#include <QLowEnergyCharacteristic>

#include <Async>

class QLowEnergyService;

class BtConnection : public QObject
{
	Q_OBJECT
public:
	explicit BtConnection(QObject *parent = 0);

	QLowEnergyService * service() const;
	QFuture<void> select(QLowEnergyService * service);

	QLowEnergyCharacteristic characteristic(const QBluetoothUuid & uuid);
	QFuture<void> write(const QLowEnergyCharacteristic & c, const QByteArray & data);
	QFuture<void> read(const QLowEnergyCharacteristic & c);
	void subscribe(const QLowEnergyCharacteristic & c);
	void notify(const QBluetoothUuid & uuid, const std::function<void(const QByteArray &)> & cb);

signals:
	void connected();
	void disconnected();
	void written();
	void read();
	void changed();

public slots:

private:
	QLowEnergyService * _service = nullptr;
	QMap<QString, std::vector<std::function<void(const QByteArray &)>>> _callbacks;
	QMap<QString, std::vector<Deferred<void>>> _writes;
	QMap<QString, std::vector<Deferred<void>>> _reads;
};

#endif // BTCONNECTION_H
