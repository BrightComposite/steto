#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QVariantMap>
#include <QUrl>
#include <QNetworkAccessManager>

#ifdef APPLABS_TASKS
	#include <Task.h>
#endif

class Http : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString url READ url NOTIFY urlChanged)
	Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)
public:
	explicit Http(QObject *parent = 0);

	QString url() const;
	QString domain() const;

	void setDomain(const QString & domain);

Q_SIGNALS:
	void urlChanged();
	void domainChanged();

public Q_SLOTS:
#ifdef APPLABS_TASKS
	void send(Task * task);
#endif

private:
	QString _domain;
	QNetworkAccessManager * _manager;
};

#endif // REMOTE_H
