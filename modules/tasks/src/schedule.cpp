#include <schedule.h>
#include <QDebug>

int Schedule::COUNT = 0;

Schedule::Consumer::Consumer() : _cb(nullptr) {

}

Schedule::Schedule() : _idx(Schedule::COUNT++) {

}

Schedule::~Schedule() {
	if(!_current.empty()) {
		qWarning() << "Smthng went wrong!" << _idx;
	}
}

Schedule * Schedule::task(const QString & key, const Job & job, const QVariantMap & data) {
	auto * s = new Schedule();
	return s->then(key, job, data);
}

Schedule * Schedule::task(const Job & job, const QVariantMap & data) {
	auto * s = new Schedule();
	return s->then(job, data);
}

Schedule * Schedule::then(const QString & key, const Job & job, const QVariantMap & data) {
	auto task = new Task(job, data);

	qDebug() << "added" << key << "to" << _idx;

	std::vector<Task *> list;
	list.push_back(task);
	_queue.push_back(list);

	return this;
}

Schedule * Schedule::then(const Job & job, const QVariantMap & data) {
	auto task = new Task(job, data);

	qDebug() << "added to" << _idx;

	std::vector<Task *> list;
	list.push_back(task);
	_queue.push_back(list);

	return this;
}

Schedule * Schedule::with(const Job & job, const QVariantMap & data) {
	if(_queue.size() == 0) {
		return then(job, data);
	}

	auto task = new Task(job, data);
	_queue.back().push_back(task);

	return this;
}

Schedule * Schedule::timeout(int t) {
	_queue.back().back()->setTimeout(t);
	return this;
}

Schedule * Schedule::success(Consumer onsuccess) {
	_succeeded = onsuccess;
	return this;
}

Schedule * Schedule::fail(Consumer onfail) {
	_failed = onfail;
	return this;
}

void Schedule::output(Task * task, int timeout) {
	success([=](Task *, Schedule * s) {
		if(s->finished()) {
			task->succeed();
		}

		return Schedule::Continue;
	})->fail([=](Task *, Schedule *) {
		task->fail();
		return Schedule::Stop;
	})->run(timeout);
}

bool Schedule::finished() const {
	return _queue.empty();
}

void Schedule::run(int timeout) {
	qDebug() << "run" << _idx;

	if(_queue.size() > 0) {
		_running = true;
		_current = std::move(_queue.front());
		_queue.erase(_queue.begin(), _queue.begin() + 1);

		for(Task * task : _current) {
			connect(task, &Task::statusChanged, [this, task]() {
				if(task->status() == Task::SUCCEEDED) {
					return succeeded(task);
				}

				if(task->status() == Task::FAILED) {
					return failed(task);
				}

				if(task->status() == Task::CANCELED) {
					return stop();
				}
			});

			try {
				task->start();
			} catch (const TaskException & e) {
				qWarning() << e.what();
				task->fail();
			}
		}
	} else {
		_running = false;
		release();
	}
}

void Schedule::stop() {
	qDebug() << "stop" << _idx;
	_running = false;

	for(auto & list : _queue) {
		for(Task * task : list) {
			task->deleteLater();
		}

		list.clear();
	}

	_queue.clear();
	release();
}

void Schedule::release() {
	for(Task * task : _current) {
		if(task) {
			task->deleteLater();
		}
	}

	_current.clear();
	deleteLater();
}

Schedule::Result Schedule::repeat(int timeout) {
	std::vector<Task *> v;

	std::accumulate(_current.begin(), _current.end(), v, [](std::vector<Task *> & v, Task * t) -> std::vector<Task *> & {
		if(t) {
			v.push_back(new Task(t->job(), t->data()));
		}

		return v;
	});

	_queue.insert(_queue.begin(), std::move(v));

	for(Task * task : _current) {
		if(task) {
			task->deleteLater();
		}
	}

	_current.clear();

	run(timeout);

	return Schedule::Break;
}

int Schedule::count() const {
	return _queue.size();
}

void Schedule::succeeded(Task * task) {
	if(!_running) {
		qWarning() << "succeeded while not running" << _idx;
		return;
	}

	qDebug() << "succeeded" << _idx;

	if(_succeeded) {
		switch(_succeeded(task, this)) {
			case Schedule::Stop:
				return stop();
			case Schedule::Break:
				return;
			default:
				break;
		}
	}

	nextTask();
}

void Schedule::failed(Task * task) {
	if(!_running) {
		qWarning() << "failed while not running" << _idx;
		return;
	}

	qDebug() << "failed" << _idx;

	if(_failed) {
		switch(_failed(task, this)) {
			case Schedule::Stop:
				return stop();
			case Schedule::Break:
				return;
			default:
				return nextTask();
		}
	}

	return stop();
}

void Schedule::nextTask() {
	qDebug() << "nextTask" << _idx;

	for(Task *& t : _current) {
		if(t) {
			if(t->status() == Task::NOT_FINISHED) {
				return;
			}

			t = nullptr;
		}
	}

	_current.clear();
	run();
}
