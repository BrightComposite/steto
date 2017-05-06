#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QBluetoothDeviceInfo>

class Device : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name NOTIFY infoChanged)
	Q_PROPERTY(QString address READ address NOTIFY infoChanged)

public:
	explicit Device();
	explicit Device(const QBluetoothDeviceInfo & info);

	const QBluetoothDeviceInfo & info() const;
	QString name() const;
	QString address() const;

	void setInfo(const QBluetoothDeviceInfo & info);

signals:
	void infoChanged();

private:
	QBluetoothDeviceInfo _info;
};

#endif // DEVICE_H
