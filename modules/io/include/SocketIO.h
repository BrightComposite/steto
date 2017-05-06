#ifndef SOCKETIO_H
#define SOCKETIO_H

#include <QObject>
#include <QUrl>
#include <QStringList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtWebSockets>

#include <QJSValue>

#include <functional>

using std::function;

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

struct SioPacket {
	int eio;
	int sio;
	QString data;
};

class SocketIO : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)
	Q_PROPERTY(QVariantMap query READ query WRITE setQuery NOTIFY queryChanged)

public:
	explicit SocketIO(QObject *parent = Q_NULLPTR);
	virtual ~SocketIO();

	const QString & sessionId() const;
	const QString & domain() const;
	QVariantMap & query();

	void setDomain(const QString & domain);
	void setQuery(const QVariantMap & query);

public Q_SLOTS:
	void open();
	void close();
	void reopen();

	void on(const QString & message, QJSValue callback);
	void emit(const QString & message, const QVariant & value);
	void emit(const QString & message, const QVariant & value, const QJSValue & callback);

Q_SIGNALS:
	void domainChanged();
	void queryChanged();
	void message(const QString & message);
	void error(const QString & reason, QString advice);
	void pongReceived();

private Q_SLOTS:
	void onError(QAbstractSocket::SocketError error);
    void onMessage(const QString & textMessage);
	void onMessage(const QByteArray & binaryMessage);
    void onSocketConnected();
    void onSocketDisconnected();

	void ping();

private:
	void poll();
	long sendMessage(const QString & contents, const QString & endpoint, bool callbackExpected);
	void acknowledge(int messageId, const QJSValue & ret = QJSValue());

	void parse(const QString & message);
	void handleEngineIO(const SioPacket & packet);
	void handleSocketIO(const SioPacket & packet);

	QNetworkRequest buildRequest(const QString & protocol) const;
	QString package(const QString & message, const QVariant & data);

	void ackReceived(int messageId, const QJSValueList & arguments);
	void event(const QString & message, const QJSValueList & arguments = QJSValueList(), bool mustAck = false, int messageId = 0);

	QWebSocket *_socket;
	QNetworkAccessManager *_manager;
	QString _domain;
	QVariantMap _query;

	int _connectionTimeout;
	int _pingInterval;
	QTimer *_pingTimer;

	QString _sessionId;
	bool _needReopen;
	bool _opened;
	bool _connected;

	QMap<QString, QJSValueList> _subscribers;
	QMap<int, QJSValue> _callbacks;
};

#endif // SOCKETIO_H
