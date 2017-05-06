#include <SocketIO.h>

#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QTimer>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <functional>

struct TimeEncoder
{
	QString encode(qint64 num) {
		QString encoded;

		do {
			encoded = alphabet[static_cast<int>(num % length)] + encoded;
			num /= length;
		} while (num > 0);

		return encoded;
	}

	int decode(const QString & str) {
		int decoded = 0;

		for (auto c : str) {
			decoded = decoded * length + alphabet.indexOf(c);
		}

		return decoded;
	}

	QString yeast() {
		auto now = encode(QDateTime::currentDateTime().currentMSecsSinceEpoch());

		if (now != prev) {
			seed = 0;
			return prev = now;
		}

		return now + "." + encode(seed++);
	}

	QString alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
	int length = 64;
	int seed = 0;
	QString prev;
};

static TimeEncoder encoder;

SocketIO::SocketIO(QObject * parent) : QObject(parent),
	_socket(new QWebSocket()), _manager(new QNetworkAccessManager()), _domain(""), _connectionTimeout(30000),
	_pingInterval(20000), _pingTimer(new QTimer()), _sessionId(), _needReopen(false), _opened(false), _connected(false)
{
	_pingTimer->setInterval(_pingInterval);

    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onMessage(QString)));
	connect(_socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onMessage(QByteArray)));
    connect(_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

	connect(_pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
}

SocketIO::~SocketIO() {
	close();
	_pingTimer->stop();
	delete _pingTimer;
	delete _socket;
	delete _manager;
}

const QString & SocketIO::sessionId() const {
	return _sessionId;
}

const QString & SocketIO::domain() const {
	return _domain;
}

QVariantMap & SocketIO::query() {
	return _query;
}

void SocketIO::setDomain(const QString & domain) {
	_domain = domain;
	Q_EMIT domainChanged();
}

void SocketIO::setQuery(const QVariantMap & query) {
	_query = query;
	Q_EMIT queryChanged();
}

void SocketIO::open() {
	if(!_opened) {
		_socket->open(buildRequest("ws"));
	}
}

void SocketIO::close() {
	if(_opened) {
		qDebug() << "close ws";
		_sessionId = "";
		_socket->sendTextMessage(QStringLiteral("12"));
		_socket->close();
	}
}

void SocketIO::reopen() {
	qDebug() << "try reopen ws";

	if(_opened) {
		_needReopen = true;
		close();
		return;
	}

	_socket->open(buildRequest("ws"));
}

void SocketIO::onError(QAbstractSocket::SocketError error) {
	qDebug() << "Error occurred: " << error;
}

void SocketIO::onMessage(const QString & msg) {
	parse(msg);
}

void SocketIO::onMessage(const QByteArray & binaryMessage) {
	qDebug() << "Binary message" << binaryMessage;
}

void SocketIO::onSocketConnected() {
	qDebug() << "ws connected";
	//_socket->sendTextMessage(QStringLiteral("52"));
}

void SocketIO::onSocketDisconnected() {
    qDebug() << "ws disconnected";
	_pingTimer->stop();
	_opened = false;

	if(_needReopen) {
		qDebug() << "reopen ws";
		open();
	}
}

void SocketIO::ping() {
	qDebug() << "socket.io send ping";
	_socket->sendTextMessage(QStringLiteral("2"));
}

void SocketIO::poll() {
	qDebug() << "ws poll...";
	_manager->get(buildRequest("http"));
}

void SocketIO::ackReceived(int messageId, const QJSValueList & arguments) {
	auto i = _callbacks.find(messageId);

	if(i != _callbacks.end()) {
		i->call(arguments);
		_callbacks.erase(i);
	}
}

void SocketIO::event(const QString & message, const QJSValueList & arguments, bool mustAck, int messageId) {
	auto i = _subscribers.find(message);

	if(i != _subscribers.end()) {
		for(auto v : *i) {
			auto ret = v.call(arguments);

            if(mustAck) {
				acknowledge(messageId, ret);
			}
		}
	}
}

QNetworkRequest SocketIO::buildRequest(const QString & protocol) const {
	QUrl url = QStringLiteral("%1://%2/socket.io/").arg(protocol).arg(_domain);
	QUrlQuery query;
	QList<QPair<QString, QString>> items;

	for(auto i = _query.begin(); i != _query.end(); ++i) {
		items.append({i.key(), i.value().toString()});
	}

	if(protocol == "ws") {
		items.append({"transport", "websocket"});
	} else {
		items.append({"transport", "polling"});
	}

	items.append({"EIO", "3"});
	items.append({"t", encoder.yeast()});

	if(_sessionId != "") {
		items.append({"sid", _sessionId});
	}

	query.setQueryItems(items);
	url.setQuery(query);

	QNetworkRequest request(url);

	if(_sessionId != "") {
		QList<QNetworkCookie> cookies;
		cookies.append(QNetworkCookie(QByteArrayLiteral("io"), _sessionId.toUtf8()));
		request.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(cookies));
	}

	if(protocol == "ws") {
		return request;
	}

	request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/plain"));
	request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("*/*"));
	request.setRawHeader(QByteArrayLiteral("Connection"), QByteArrayLiteral("close"));

	return request;
}

void SocketIO::parse(const QString & message) {
	QRegExp regExp("^([0-6])([0-6]?)(.*)$", Qt::CaseInsensitive, QRegExp::RegExp2);

	if (regExp.indexIn(message) == -1) {
		qWarning() << "Can't parse ws message: " << message;
		return;
	}

	QStringList captured = regExp.capturedTexts();
	SioPacket packet;
	packet.eio = captured[1].toInt();
	packet.sio = captured[2].toInt();
	packet.data = captured[3];

	handleEngineIO(packet);
}

void SocketIO::handleEngineIO(const SioPacket & packet) {
	auto & data = packet.data;

	switch(packet.eio) {
		case 0: { //connected
			qDebug() << "engine.io opened:" << data;
			_opened = true;

			if(!_connected) {
				QJsonParseError parseError;
				QJsonDocument document = QJsonDocument::fromJson(data.toLatin1(), &parseError);

				if (parseError.error != QJsonParseError::NoError) {
					qDebug() << parseError.errorString();
					return;
				}

				auto obj = document.object();
				_pingInterval = obj["pingInterval"].toInt() - 500;
				_connectionTimeout = obj["pingTimeout"].toInt();
				_sessionId = obj["sid"].toString();

				_pingTimer->setInterval(_pingInterval);
			}

			break;
		}

		case 1: {	//disconnected
			qDebug() << "engine.io closed";
			_sessionId = "";
			_connected = false;
			_socket->close();
			break;
		}

		case 2: {
			qDebug() << "engine.io ping received";
			break;
		}

		case 3: {
			qDebug() << "engine.io pong received";
			Q_EMIT pongReceived();
			break;
		}

		case 4: {	//message
			qDebug() << "engine.io message received";
			handleSocketIO(packet);
			Q_EMIT message(data);
			break;
		}

		case 5: {	//upgrade
			qDebug() << "engine.io upgrade received:" << data;
			break;
		}

		case 6: { //noop
			qDebug() << "engine.io noop received";
			break;
		}
	}
}

void SocketIO::handleSocketIO(const SioPacket & packet) {
	auto & data = packet.data;

	switch(packet.sio) {
		case 0: { //connected
			qDebug() << "socket.io connected";
			event("connected");
			_connected = true;
			_pingTimer->start();
			ping();
			break;
		}

		case 1: {	//disconnected
			qDebug() << "socket.io disconnected";
			event("disconnected");
			break;
		}

		case 2: { // event
            qDebug() << "socket.io event received:" << data;
			QJsonParseError parseError;
			QJsonDocument document = QJsonDocument::fromJson(QByteArray(data.toUtf8()), &parseError);

			if (parseError.error != QJsonParseError::NoError) {
				qWarning() << parseError.errorString();
				return;
			}

            if (!document.isArray()) {
				qWarning() << "Json document is not an array";
				return;
			}

            QJsonArray array = document.array();
            QJsonValue value = array[0];

			if (value.isUndefined()) {
				qWarning() << "Invalid event received: no name";
			}

			QString message = value.toString();
            QJsonValue args = array[1];
			QJsonArray arr;
			QJsonDocument doc;
			arr.append(args);
			doc.setArray(arr);
			QJSValueList arguments;
			arguments << QJSValue(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
			event(message, arguments, false, 0);
			break;
		}

		case 3: { //ack
            qDebug() << "socket.io ack received:" << data;
			QRegExp regExp("^([0-9]+)(\\+)?(.*)$", Qt::CaseInsensitive, QRegExp::RegExp2);

			if (regExp.indexIn(data) != -1) {
				QJsonParseError parseError;
				QJSValueList arguments;
				int messageId = regExp.cap(1).toInt();
				QString args = regExp.cap(3);

				if (!args.isEmpty()) {
					QJsonDocument doc = QJsonDocument::fromJson(args.toLatin1(), &parseError);

					if (parseError.error != QJsonParseError::NoError) {
						qWarning() << "JSONParseError:" << parseError.errorString();
						return;
					}

					if (doc.isArray()) {
						for(auto v : doc.array()) {
							arguments << v.toVariant().value<QJSValue>();
						}
					} else {
						qWarning() << "Error: data of event is not an array";
						return;
					}
				}

				ackReceived(messageId, arguments);
			}
			break;
		}

		case 4: {	//error
			qDebug() << "socket.io error received";
			QStringList pieces = data.split("+");
			QString reason = pieces[0];
			QString advice;

			if (pieces.length() == 2) {
				advice = pieces[1];
			}

			Q_EMIT error(reason, advice);
			break;
		}

		case 5: {	//binary_event
			qDebug() << "socket.io binary_event received:" << data;
			break;
		}

		case 6: { //binary_ack
			qDebug() << "socket.io binary_ack received:" << data;
			break;
		}
	}
}

QString SocketIO::package(const QString & message, const QVariant &value) {
	QJsonDocument document;
	QJsonArray array;

	array.append(QJsonValue(message));

	if (value.canConvert<QVariantMap>()) {
		array.append(QJsonObject::fromVariantMap(value.toMap()));
	} else if (value.canConvert<QVariantList>()) {
		array.append(QJsonArray::fromVariantList(value.toList()));
	} else if (value.type() == QVariant::Int){
		array.append(QJsonValue(value.toInt()));
	} else if (value.type() == QVariant::Bool){
		array.append(QJsonValue(value.toBool()));
	} else {
		array.append(QJsonValue(value.toString()));
	}

	document.setArray(array);
	return QString::fromUtf8(document.toJson(QJsonDocument::Compact));
}

void SocketIO::on(const QString & message, QJSValue callback) {
	if(message == "connected" && _opened) {
		callback.call();
	}

	_subscribers[message] << callback;
}

void SocketIO::emit(const QString & message, const QVariant & value) {
	sendMessage(package(message, value), "", true);
}

void SocketIO::emit(const QString & message, const QVariant & value, const QJSValue & callback) {
	auto id = sendMessage(package(message, value), "", true);
	_callbacks[id] = callback;
}

void SocketIO::acknowledge(int messageId, const QJSValue & ret)
{
	QString msg = QStringLiteral("43") + QString::number(messageId);

	if (!ret.isUndefined() && !ret.isNull()) {
		auto doc = QJsonDocument::fromVariant(ret.toVariant());
		msg.append(QStringLiteral("+") + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
	}

	_socket->sendTextMessage(msg);
}

long SocketIO::sendMessage(const QString & contents, const QString & endpoint, bool callbackExpected) {
	static long id = 0;
	const QString msg = QStringLiteral("42%1").arg(contents);

	qDebug() << "socket.io send message:" << contents;

	_socket->sendTextMessage(msg);
	return id;
}
