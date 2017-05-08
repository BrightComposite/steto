#ifndef APPLABS_TASK_H
#define APPLABS_TASK_H

#include <QObject>
#include <QVariantMap>
#include <QTimer>
#include <QJSValue>
#include <QJSEngine>
#include <functional>
#include <type_traits>
#include <sstream>
#include <memory>

class AsyncChain;

class TaskException : public std::runtime_error
{
private:
	template<class H, class ... T, typename =
			 typename std::enable_if<
				 !std::is_same<
					 typename std::decay<H>::type,
					 std::stringstream
					 >::value
				 >::type
			 >
	static std::string & construct(H && h, T &&... t) {
		std::stringstream ss;
		ss << h;
		return construct(ss, std::forward<T>(t)...).str();
	}

	template<class H, class ... T>
	static std::stringstream & construct(std::stringstream & ss, H && h, T &&... t) {
		ss << " " << h;
		return construct(ss, std::forward<T>(t)...);
	}

	static std::stringstream & construct(std::stringstream & ss) {
		return ss;
	}

public:
	template<class H, class ... T>
	TaskException(H && h, T &&... t) : std::runtime_error(construct(std::forward<H>(h), std::forward<T>(t)...)) {}
};

class Task : public QObject
{
	Q_OBJECT
public:
	struct Job {
		Job();

		template<class F,
				 typename = typename std::enable_if<
					 std::is_same<
						 decltype(std::declval<F>()(static_cast<Task *>(nullptr))),
						 bool>::value>::type, int = 0>
		Job(const F & worker) {
			_worker = worker;
		}

		template<class F,
				typename = typename std::enable_if<
					!std::is_same<
						decltype(std::declval<F>()(static_cast<Task *>(nullptr))),
						bool>::value>::type>
		Job(const F & worker) {
			_worker = [worker](Task * task) mutable {
				worker(task);
				return false;
			};
		}

		bool operator () (Task * task) const {
			return _worker(task);
		}

	private:
		std::function<bool(Task *)> _worker;
	};

	enum Status {
		NOT_FINISHED,
		SUCCEEDED,
		FAILED,
	};
	Q_ENUM(Status)

	explicit Task();
	explicit Task(const Job & job, const QVariantMap & data);
	virtual ~Task();

	Job & job();
	QVariantMap & data();
	Status status() const;
	QTimer * timer() const;

	void setData(const QVariantMap & data);
	void setTimeout(int timeout);

	void start();

	Task * write(const QVariantMap & obj);
	QVariant read(const QString & key);

	void succeed();
	void fail();

Q_SIGNALS:
	void dataChanged();
	void statusChanged();
	void timerChanged();

private:
	void resetTimer();

	Job _job;
	QTimer * _timer = nullptr;
	QVariantMap _data;
	Status _status = Status::NOT_FINISHED;
};

#endif // SCHEDULE_H
