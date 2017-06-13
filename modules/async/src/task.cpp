#include <task.h>
#include <taskqueue.h>
#include <QDebug>

Task::Job::Job() : _worker(nullptr) {

}

Task::Task() {}

Task::Task(const Job & job, const QVariantMap & data) : _job(job), _data(data) {
	qDebug() << "Created task" << this;
}

Task::~Task() {
	qDebug() << "Deleted task" << this;
}

QTimer * Task::timer() const {
	return _timer;
}

Task::Job & Task::job() {
	return _job;
}

QVariantMap & Task::data() {
	return _data;
}

Task::Status Task::status() const {
	return _status;
}

void Task::setData(const QVariantMap & data) {
	_data = data;
	Q_EMIT dataChanged();
}

void Task::resetTimer() {
	if(_timer) {
		_timer->stop();
		delete _timer;
		_timer = nullptr;
	}
}

void Task::setTimeout(int timeout) {
	resetTimer();

	_timer = new QTimer();
	_timer->setInterval(timeout);

	connect(_timer, &QTimer::timeout, [this] () {
		delete _timer;
		_timer = nullptr;
		fail();
	});
}


Task * Task::write(const QVariantMap & obj) {
	for(auto & key : obj.keys()) {
		_data[key] = obj[key];
	}

	Q_EMIT dataChanged();
	return this;
}

QVariant Task::read(const QString & key) {
	return _data[key];
}

void Task::start() {
	if(_timer) {
		_timer->start();
	}

	if(_job(this)) {
		return succeed();
	}
}

void Task::succeed() {
	qDebug() << "succeed task" << this;
	resetTimer();

	_status = Task::SUCCEEDED;
	Q_EMIT statusChanged();
	delete this;
}

void Task::fail() {
	qDebug() << "fail task" << this;
	resetTimer();

	_status = Task::FAILED;
	Q_EMIT statusChanged();
	delete this;
}
