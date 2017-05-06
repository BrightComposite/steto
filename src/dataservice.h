#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <QObject>
#include <BtConnection>

class DataService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool active READ active NOTIFY activeChanged)
public:
	explicit DataService(BtConnection * connection = nullptr);

	bool active() const;

signals:
	void activeChanged();

public slots:
	void toggle();
	void activate();
	void deactivate();

private:
	bool _active;
	BtConnection * _connection;
};

#endif // IOSERVICE_H
