#ifndef APPLABS_TASK_POOL_H
#define APPLABS_TASK_POOL_H

#include <QObject>
#include <task.h>

#include <map>
#include <vector>

class TaskPool : public QObject
{
	Q_OBJECT
public:
	explicit TaskPool(QObject *parent = 0);

	bool pending(int key);

	void add(int key, Task * r);
	void succeeded(int key);
	void failed(int key);
	void canceled(int key);
	void failedAll();
	void canceledAll();

Q_SIGNALS:

public Q_SLOTS:

private:
	std::map<int, std::vector<Task *>> _tasks;
};

#endif // EVENTPOOL_H
