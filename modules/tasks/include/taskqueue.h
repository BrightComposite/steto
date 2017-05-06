#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <QObject>
#include <asyncfuture.h>

namespace async = AsyncFuture;

using async::Deferred;

template<class T>
struct AsyncBuilder {};

template<class T>
struct return_to_arg {
	using type = T &&;
};

template<class T>
struct return_to_arg<T &> {
	using type = T &;
};

template<>
struct return_to_arg<void> {
	using type = void;
};

template<class ... A>
struct arg_to_return {};

template<class T>
struct arg_to_return<T>
{
	using type = T;
};

template<>
struct arg_to_return<> {
	using type = void;
};

template<class T>
struct remove_deferred {
	using type = T;
};

template<class T>
struct remove_deferred<Deferred<T>> {
	using type = T;
};

template<class R, class T>
struct func {
	using type = R(T);
};

template<class R>
struct func<R, void> {
	using type = R();
};

template<class T>
using return_to_arg_t = typename return_to_arg<T>::type;

template<class ... A>
using arg_to_return_t = typename arg_to_return<A...>::type;

template<class T>
using remove_deferred_t = typename remove_deferred<T>::type;

template<class F>
using result_of_t = remove_deferred_t<typename std::result_of<F>::type>;

template<class R, class T>
using func_t = typename func<R, T>::type;

namespace AsyncFuture {
	template<class T>
	inline Deferred<T> complete(T t) {
		auto d = deferred<T>();
		d.complete(t);
		return d;
	}

	inline Deferred<void> complete() {
		auto d = deferred<void>();
		d.complete();
		return d;
	}

	template<class T>
	inline Deferred<T> cancel() {
		auto d = deferred<T>();
		d.cancel();
		return d;
	}

	inline Deferred<void> cancel() {
		auto d = deferred<void>();
		d.cancel();
		return d;
	}

	template<class T>
	inline Deferred<T> subscribe(const Observable<T> & complete) {
		auto d = deferred<T>();
		d.complete(complete.future());

		return d;
	}

	template<class T>
	inline Deferred<T> subscribe(const Observable<T> & complete, const Observable<T> & cancel) {
		auto d = deferred<T>();
		d.complete(complete.future());
		d.cancel(cancel.future());

		return d;
	}
}

class TaskQueue : public QObject
{
	template<class T>
	friend class AsyncBuilder;

	Q_OBJECT
public:
	explicit TaskQueue() {}
	virtual ~TaskQueue() {}

	template<class F, class ... A, typename FR = decltype(std::declval<F>()(std::declval<A>()...))>
	static AsyncBuilder<remove_deferred_t<FR>()> * future(F f, A &&... args) {
		auto * q = new TaskQueue;
		auto * b = new AsyncBuilder<remove_deferred_t<FR>()>(q, std::bind(f, std::forward<A>(args)...));
		q->_first = std::bind(&AsyncBuilder<remove_deferred_t<FR>()>::execute, b);
		return b;
	}

	template<class T, class R, class ... A>
	static AsyncBuilder<R()> * future(T * object, Deferred<R>(T::*method)(A &&...), A &&... args) {
		auto * q = new TaskQueue;
		auto * b = new AsyncBuilder<R()>(q, std::bind(method, object, std::forward<A>(args)...));
		q->_first = std::bind(&AsyncBuilder<R()>::execute, b);
		return b;
	}

	TaskQueue * success(const std::function<void()> & onsuccess) {
		_succeeded = onsuccess;
		return this;
	}

	TaskQueue * fail(const std::function<void()> & onfail) {
		_failed = onfail;
		return this;
	}

	void run() {
		_first();
	}

private:
	void succeeded() {
		if(_succeeded) {
			_succeeded();
		}

		delete this;
	}

	void failed() {
		if(_failed) {
			_failed();
		}

		delete this;
	}

	std::function<void()> _first;
	std::function<void()> _succeeded, _failed;
};

template<class ... A>
class AsyncBuilderBase
{
public:
	virtual void execute(A &&... args) = 0;
	virtual void cancel() = 0;
};

template<>
class AsyncBuilderBase<void>
{
public:
	virtual ~AsyncBuilderBase() {}

	virtual void execute() = 0;
	virtual void cancel() = 0;
};

template<class R, class ... A>
class AsyncBuilder<R(A...)> : public AsyncBuilderBase<arg_to_return_t<A...>>
{
	using Job = std::function<async::Deferred<R>(A &&...)>;
	using Result = return_to_arg_t<R>;
	using Then = std::function<func_t<void, Result>>;

public:
	AsyncBuilder(TaskQueue * queue, const Job & f) : _queue(queue), f(f) {}
	virtual ~AsyncBuilder() {}

	virtual void execute(A &&... args) override {
		perform(std::forward<A>(args)...);
	}

	virtual void cancel() {
		if(_then) {
			_then->cancel();
		}

		delete this;
	}

	template<class F, class ... X, typename FR = decltype(std::declval<F>()(std::declval<X>()...)), typename = typename std::enable_if<std::is_same<Result, void>::value>::type>
	AsyncBuilder<func_t<remove_deferred_t<FR>, Result>> * then(const F & f, X && ... args) {
		auto * b = new AsyncBuilder<func_t<remove_deferred_t<FR>, Result>>(_queue, std::bind(f, std::forward<X>(args)...));
		_then = b;
		return b;
	}

	template<class F, class ... X, typename FR = decltype(std::declval<F>()(std::declval<Result>(), std::declval<X>()...))>
	AsyncBuilder<func_t<remove_deferred_t<FR>, Result>> * then(const F & f, X && ... args) {
		auto * b = new AsyncBuilder<func_t<remove_deferred_t<FR>, Result>>(_queue, std::bind(f, std::placeholders::_1, std::forward<X>(args)...));
		_then = b;
		return b;
	}

	TaskQueue * queue() {
		return _queue;
	}

	TaskQueue * success(const std::function<void()> & onsuccess) {
		return _queue->success(onsuccess);
	}

	TaskQueue * fail(const std::function<void()> & onfail) {
		return _queue->fail(onfail);
	}

	void run() {
		_queue->run();
	}

	template<typename Rslt = Result, typename = typename std::enable_if<std::is_same<Rslt, void>::value>::type>
	void perform(A &&... args) {
		auto d = f(std::forward<A>(args)...);

		if(_then) {
			if(d.future().isFinished()) {
				_queue->succeeded();
				_then->execute();
				delete this;
			} else if(d.future().isCanceled()) {
				cancel();
			} else {
				d.subscribe([this, d]() {
					_queue->succeeded();
					_then->execute();
					delete this;
				}, [this](){
					cancel();
				});
			}
		}
	}

	template<typename Rslt = Result, typename = typename std::enable_if<!std::is_same<Rslt, void>::value>::type, int = 0>
	void perform(A &&... args) {
		auto d = f(std::forward<A>(args)...);

		if(_then) {
			if(d.future().isFinished()) {
				_queue->succeeded();
				_then->execute(d.future());
				delete this;
			} else if(d.future().isFinished()) {
				_queue->succeeded();
				_then->execute(d.future());
				delete this;
			} else {
				d.subscribe([this, d]() {
					_queue->succeeded();
					_then->execute(d.future());
					delete this;
				}, [this](){
					cancel();
				});
			}
		}
	}

private:
	TaskQueue * _queue;
	Job f;
	AsyncBuilderBase<Result> * _then = nullptr;
};

#endif // SCHEDULEBUILDER_H
