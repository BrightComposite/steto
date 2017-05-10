#include "dataservice.h"
#include <DeviceService>
#include <QDebug>

DataService::DataService(BtConnection * connection) : QObject(connection), _connection(connection) {
	_timer = new QTimer(this);
	_timer->setInterval(60);

	connect(_connection, &BtConnection::disconnected, [this]() {
		if(_active) {
			_active = false;
			emit activeChanged();
		}
	});

	connect(_timer, &QTimer::timeout, [this]() {
		if(_displayed.size() > DISPLAYED_SIZE) {
			_displayed.erase(_displayed.begin(), _displayed.begin() + (_displayed.size() - 128));
		}

		emit valuesFlushed();
	});

	connect(this, &DataService::activeChanged, [this]() {
		if(_active) {
			_timer->start();
			emit valuesFlushed();
		} else {
			_timer->stop();
		}
	});

	connect(this, &DataService::completed, [this]() {
		deactivate();

		float out[SIZE];
		_fft.do_fft(out, _collected.data());
		_fft.rescale(out);

		_spectre.clear();

		for(int i = 0; i < DISPLAYED_SIZE; ++i) {
			int a = SIZE / 2 - std::abs(SIZE / 2 - i);
			int b = SIZE - std::abs(SIZE / 2 - i);

			float f = a % (SIZE / 2) == 0 ? std::abs(out[a]) : std::sqrt(out[a] * out[a] + out[b] * out[b]);
			_spectre.push_back(f);
			qDebug() << f;
		}

		emit spectreChanged();
		emit valuesFlushed();
	});

	_connection->notify(DeviceService::IO_CHARACTERISTIC, [this](const QByteArray & value) {
		qDebug() << value;

		if(_active) {
			int v = 0;
			int counter = 0;

			for(auto c : value) {
				v = c - 100;
				_collected.push_back(v);

				if(++counter == 8) {
					_displayed.push_back(v);
					counter = 0;
				}

				if(_collected.size() == SIZE) {
					emit completed();
					return;
				}
			}
		}
	});
}

bool DataService::active() const {
	return _active;
}

int DataService::count() const {
	return _collected.size();
}

float DataService::progress() const {
	return float(count()) / SIZE;
}

const QList<int> & DataService::displayed() const {
	return _displayed;
}

const QList<qreal> & DataService::spectre() const {
	return _spectre;
}

void DataService::start() {
	if(_active) {
		deactivate();
	}

	_collected.clear();
	_displayed.clear();
	emit valuesFlushed();

	async::chain(this, &DataService::activate)
	-> then(async::observe(this, &DataService::completed))
	-> run();
}

void DataService::setDisplayRange(int offset, uint count, uint samplePeriod) {
	if(samplePeriod > 256) {
		samplePeriod = 256;
	}

	if(count * samplePeriod > _collected.size()) {
		count = _collected.size() / samplePeriod;
	}

	if(offset < 0) {
		offset = 0;
	} else if(offset + count * samplePeriod > _collected.size()) {
		offset = _collected.size() - count * samplePeriod;
	}

	_displayed.clear();

	for(int i = offset; i < offset + count * samplePeriod; i += samplePeriod) {
		_displayed.push_back(int(_collected[i]));
	}

	emit valuesFlushed();
}

QFuture<void> DataService::activate() {
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

QFuture<void> DataService::deactivate() {
	if(_connection->service() == nullptr) {
		qWarning() << "Connection is invalid";
		return async::cancel();
	}

	_active = false;
	emit activeChanged();

	auto c = _connection->characteristic(DeviceService::IO_CHARACTERISTIC);
	return _connection->write(c, "A0");
}

bool DataService::serialize(const QString & filepath) {
	if(_collected.size() != SIZE) {
		qWarning() << "There is not valid data to save to file" << filepath;
		return false;
	}

	QFile file(filepath);

	if(!file.open(QFile::WriteOnly)) {
		qWarning() << "Can't open file" << filepath << "for write";
		return false;
	}

	QByteArray data;

	for(auto val : _collected) {
		data.append(QByteArray::number(int(val)) + "\n");
	}

	file.write(data);
	file.close();

	return true;
}

bool DataService::unserialize(const QString & filepath) {
	QFile file(filepath);

	if(!file.open(QFile::ReadOnly)) {
		qWarning() << "Can't open file" << filepath << "for read";
		return false;
	}

	_collected.clear();
	auto bytes = file.readAll();
	file.close();

	for(auto & v : bytes.split('\n')) {
		if(v.size() > 0) {
			_collected.push_back(float(v.toInt()));
		}
	}

	emit completed();
	return true;
}
