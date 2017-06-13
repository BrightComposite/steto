#include <taskpool.h>
#include <QDebug>

TaskPool::TaskPool(QObject *parent) : QObject(parent)
{

}

bool TaskPool::pending(int key) {
	return _tasks.find(key) != _tasks.end();
}

void TaskPool::add(int key, Task * t) {
	_tasks[key].push_back(t);
	qDebug() << "Added task" << key;
}

void TaskPool::succeeded(int key) {
	auto i = _tasks.find(key);

	if(i != _tasks.end()) {
		qDebug() << "Succeeded task" << key;
		auto & list = i->second;

		for(Task * t : list) {
			t->succeed();
		}

		list.clear();
		_tasks.erase(i);
	}
}

void TaskPool::failed(int key) {
	auto i = _tasks.find(key);
	qDebug() << "Tasks pending:" << _tasks.size();

	if(i != _tasks.end()) {
		qDebug() << "Failed task" << key;
		auto & list = i->second;

		for(Task * t : list) {
			qDebug() << "Remove task" << key;
			t->fail();
		}

		list.clear();
		_tasks.erase(i);
	}
}

void TaskPool::failedAll() {
	qDebug() << "Failed all tasks";
	for(auto & entry : _tasks) {
		for(auto & t : entry.second) {
			t->fail();
		}

		entry.second.clear();
	}

	_tasks.clear();
}
