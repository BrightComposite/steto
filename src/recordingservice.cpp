#include "recordingservice.h"
#include <DeviceService>
#include <models/SamplesModel>

RecordingService::RecordingService(BtConnection * connection) : QObject(connection), _connection(connection) {
	_timer = new QTimer(this);
	_timer->setInterval(300);

	_provider = new SampleProvider(this);

	connect(_connection, &BtConnection::disconnected, [this]() {
		if(_active) {
			_active = false;
			emit activeChanged();
		}
	});

	connect(_timer, &QTimer::timeout, [this]() {
		_provider->signal()->setRawOffset(_collected.size() - _provider->signal()->count());
		_provider->setData(_collected, 964);
		emit progressChanged();
	});

	connect(this, &RecordingService::activeChanged, [this]() {
		if(_active) {
			_timer->start();
			emit activated();
		} else {
			_timer->stop();
			emit deactivated();
		}
	});

	_connection->notify(DeviceService::IO_CHARACTERISTIC, [this](const QByteArray & value) {
		qDebug() << value;

		if(_active) {
			int v = 0;

			for(auto c : value) {
				v = int(c) - 116;
				_collected.push_back(v);

				if(_collected.size() == SIZE) {
					deactivate();
					emit completed();
					return;
				}
			}
		}
	});
}

bool RecordingService::isActive() const {
	return _active;
}

bool RecordingService::isCompleted() const {
	return _collected.size() == SIZE;
}

float RecordingService::progress() const {
	return float(_collected.size()) / SIZE;
}

SampleProvider * RecordingService::provider() const {
	return _provider;
}

void RecordingService::start() {
	stop();

	async::chain(this, &RecordingService::activate)
	-> then(async::observe(this, &RecordingService::completed))
	-> cancel_on(this, &RecordingService::deactivated)
	-> run();
}

void RecordingService::stop() {
	if(_active) {
		deactivate();
	}

	reset();
}

void RecordingService::reset() {
	_collected.clear();
	_provider->setData(_collected, 1);
}

QFuture<void> RecordingService::activate() {
	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return async::cancel();
	}

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	auto d = async::deferred<void>();

	async::chain(_connection, &BtConnection::write, c, "A1")
	-> success_with([=]() {
		_active = true;
		emit activeChanged();
	})
	-> subscribe(d)
	-> run();

	return d.future();
}

QFuture<void> RecordingService::deactivate() {
	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return async::cancel();
	}

	_active = false;
	emit activeChanged();

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	return _connection->write(c, "A0");
}
