#ifndef SCHEDULEBUILDER_H
#define SCHEDULEBUILDER_H

#include <QObject>
#include <QJSValue>
#include <QTimer>
#include <task.h>

class Schedule : public QObject {
	Q_OBJECT
public:
	enum Result {
		Stop = 0,
		Continue = 1,
		Break = 2
	};
	Q_ENUM(Result)

	using Job = typename Task::Job;

	struct Consumer {
		Consumer();

		template<class F,
				 typename = typename std::enable_if<
					 std::is_same<
						 decltype(std::declval<F>()(static_cast<Task *>(nullptr), static_cast<Schedule *>(nullptr))),
						 Result>::value>::type, int = 0>
		Consumer(const F & cb) {
			_cb = cb;
		}

		template<class F,
				typename = typename std::enable_if<
					!std::is_same<
						decltype(std::declval<F>()(static_cast<Task *>(nullptr), static_cast<Schedule *>(nullptr))),
						Result>::value>::type>
		Consumer(const F & cb) {
			_cb = [cb](Task * task, Schedule * builder) mutable {
				cb(task, builder);
				return Result::Continue;
			};
		}

		operator bool () const {
			return _cb != nullptr;
		}

		Result operator () (Task * task, Schedule * builder) const {
			return _cb(task, builder);
		}

	private:
		std::function<Result(Task *, Schedule *)> _cb;
	};

	explicit Schedule();
	virtual ~Schedule();

	static Schedule * task(const QString & key, const Job & job, const QVariantMap & data = {});
	static Schedule * task(const Job & job, const QVariantMap & data = {});

	Schedule * then(const QString & key, const Job & job, const QVariantMap & data = {});
	Schedule * then(const Job & job, const QVariantMap & data = {});
	Schedule * with(const Job & job, const QVariantMap & data = {});

	Schedule * success(Consumer onsuccess);
	Schedule * fail(Consumer onfail);

	Schedule * timeout(int t);

	bool finished() const;
	int count() const;

	void run(int timeout = 1);
	void output(Task * to, int timeout = 1);

	Result repeat(int timeout = 1);

private:
	void stop();

	void succeeded(Task * task);
	void failed(Task * task);

	void nextTask();
	void release();

	std::vector<std::vector<Task *>> _queue;
	Consumer _succeeded, _failed;
	std::vector<Task *> _current;
	bool _running = false;

	int _idx;

	static int COUNT;
};

#endif // SCHEDULEBUILDER_H
