#include <Http.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>

#include <QDebug>

Http::Http(QObject *parent) : QObject(parent) {
	_manager = new QNetworkAccessManager(this);
}

QString Http::url() const {
	return "http://" + _domain + "/";
}

QString Http::domain() const {
	return _domain;
}

void Http::setDomain(const QString & domain) {
	_domain = domain;
	Q_EMIT domainChanged();
	Q_EMIT urlChanged();
}

#ifdef APPLABS_TASKS
void Http::send(Task * task) {
	auto & data = task->data();
	auto route = data["command"].toString();
	auto method = data["method"].toString();

	auto & params = data["params"];
	auto & files = data["files"];
	auto & headers = data["headers"];

	QUrlQuery query;

	if(params.isValid()) {
		auto p = qvariant_cast<QVariantMap>(params);

		for(auto i = p.begin(); i != p.end(); ++i) {
			query.addQueryItem(i.key(), i.value().toString());
		}
	}

	if(files.isValid()) {
		auto f = qvariant_cast<QVariantMap>(files);

		for(auto i = f.begin(); i != f.end(); ++i) {
			query.addQueryItem(i.key(), i.value().toString());
		}
	}

	if(query.isEmpty()) {
		qDebug() << "Request url:" << route << " method:" << method;
	} else {
		qDebug() << "Request url:" << route << " method:" << method << " args:" << query.toString();
	}

	QUrl url(this->url() + route);
	QNetworkReply * reply = nullptr;

	if(method == "GET") {
		if(!query.isEmpty()) {
			url.setQuery(query);
		}
	}

	QNetworkRequest request(url);

	if(headers.isValid()) {
		auto h = qvariant_cast<QVariantMap>(headers);

		for(auto i = h.begin(); i != h.end(); ++i) {
			request.setRawHeader(i.key().toLatin1(), i.value().toString().toLatin1());
		}
	}

	if(method == "GET") {
		reply = _manager->get(request);
	} else {
		QByteArray bytes;
		QByteArray boundary = "boundary_.oOo._";

		if(files.isValid()) {
			for(int i = 0; i < 10; ++i) {
				boundary += QByteArray::number(qrand() % 16, 16);
			}

			if(params.isValid()) {
				auto p = qvariant_cast<QVariantMap>(params);

				for(auto i = p.begin(); i != p.end(); ++i) {
					qDebug() << i.key() << i.value().toString().toLatin1();

					bytes.append("--" + boundary);
					bytes.append("\r\n");
					bytes.append("Content-Disposition: form-data; name=\"" + i.key().toUtf8() + "\"");
					bytes.append("\r\n\r\n");
					bytes.append(i.value().toString().toUtf8());
					bytes.append("\r\n");
				}
			}

			auto f = qvariant_cast<QVariantMap>(files);

			QMimeDatabase db;

			for(auto i = f.begin(); i != f.end(); ++i) {
				auto name = i.key();
				auto filename = i.value().toString();
				QFile file(filename);
				QFileInfo info(file);

				auto type = db.mimeTypeForFile(info);

				if(!file.open(QIODevice::ReadOnly)) {
					qWarning() << "Can't open file:" << filename;
					continue;
				}

				bytes.append("--" + boundary);
				bytes.append("\r\n");
				bytes.append("Content-Disposition: form-data; name=\"" + name.toUtf8() + "\"; filename=\"" + info.fileName().toUtf8() + "\"");
				bytes.append("\r\n");
				bytes.append("Content-Type: " + type.name().toUtf8());
				bytes.append("\r\n\r\n");
				bytes.append(file.readAll());
				bytes.append("\r\n");
			}

			bytes.append("--" + boundary + "--\r\n");
			qDebug() << bytes;
		} else {
			bytes = query.toString(QUrl::FullyEncoded).toUtf8();
		}

		if(files.isValid()) {
			request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary);
		} else {
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		}

		request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(bytes.length()));

		reply = method == "POST" ?
			_manager->post(request, bytes) :
			method == "PUT" ?
				_manager->put(request, bytes) :
				nullptr;
	}

	if(reply == nullptr) {
		qWarning() << "Unknown method!";
		task->fail();
	}

	connect(reply, &QNetworkReply::finished, [reply, task]() {
		auto & d = task->data();
		auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

		if(status != 200) {
			reply->deleteLater();
			d["status"] = status;
			Q_EMIT task->dataChanged();
			return task->fail();
		}

		auto responseData = reply->readAll();
		reply->deleteLater();

		qDebug() << "Status:" << status;
		d["status"] = status;

		auto svas = d.find("saveAs");

		if(svas != d.end()) {
			d["response"] = "";
			Q_EMIT task->dataChanged();

			QFile file(svas.value().toString());
			file.open(QFile::WriteOnly);
			file.write(responseData);

			return task->succeed();
		}

		auto response = QString::fromUtf8(responseData);

		d["response"] = response;
		Q_EMIT task->dataChanged();
		return task->succeed();
	});

	connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [reply, task](QNetworkReply::NetworkError e) {
		auto & d = task->data();
		auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		auto response = QString::fromUtf8(reply->readAll());
		reply->deleteLater();

		qDebug() << "Status:" << status;
		qDebug() << e;
		d["status"] = status;
		d["response"] = response;
		Q_EMIT task->dataChanged();
		task->fail();
	});
}
#endif
