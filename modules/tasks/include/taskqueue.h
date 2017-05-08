#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <QObject>
#include <asyncfuture.h>

namespace async = AsyncFuture;

using async::Deferred;
using async::Observable;

template<class T>
struct AsyncTask {};

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
struct remove_future {
	using type = T;
};

template<class T>
struct remove_future<QFuture<T>> {
	using type = T;
};

template<class T>
struct is_future_impl {
	static constexpr bool value = false;
};

template<class T>
struct is_future_impl<QFuture<T>> {
	static constexpr bool value = true;
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
using remove_future_t = typename remove_future<T>::type;

template<class T>
using is_future = is_future_impl<typename std::decay<T>::type>;

template<class F>
using result_of_t = remove_future_t<typename std::result_of<F>::type>;

template<class R, class T>
using func_t = typename func<R, T>::type;

namespace AsyncFuture {
	template<class T>
	inline QFuture<T> complete(T t) {
		auto d = deferred<T>();
		d.complete(t);
		return d.future();
	}

	inline QFuture<void> complete() {
		auto d = deferred<void>();
		d.complete();
		return d.future();
	}

	template<class T>
	inline QFuture<T> cancel() {
		auto d = deferred<T>();
		d.cancel();
		return d.future();
	}

	inline QFuture<void> cancel() {
		auto d = deferred<void>();
		d.cancel();
		return d.future();
	}

	template<class T>
	inline QFuture<T> cancel(const QFuture<T> & future) {
		auto d = deferred<void>();
		d.cancel(future);
		return d.future();
	}

	template<class T>
	inline QFuture<T> cancel(const Observable<T> & o) {
		auto d = deferred<void>();
		d.cancel(o.future());
		return d.future();
	}

	template<class T>
	inline QFuture<T> connect(const QFuture<T> & complete) {
		return complete;
	}

	template<class T>
	inline QFuture<T> connect(const Observable<T> & complete) {
		return complete.future();
	}

	template<class T>
	inline QFuture<T> connect(const QFuture<T> & complete, const QFuture<T> & cancel) {
		auto d = deferred<T>();
		d.complete(complete);
		d.cancel(cancel);

		return d.future();
	}

	template<class T>
	inline QFuture<T> connect(const QFuture<T> & complete, const Observable<T> & cancel) {
		return connect(complete, cancel.future());
	}

	template<class T>
	inline QFuture<T> connect(const Observable<T> & complete, const QFuture<T> & cancel) {
		return connect(complete.future(), cancel);
	}

	template<class T>
	inline QFuture<T> connect(const Observable<T> & complete, const Observable<T> & cancel) {
		return connect(complete.future(), cancel.future());
	}

	template<class Ctx, class T, class Complete, class Cancel>
	inline void subscribe(Ctx * context, const QFuture<T> & future, Complete && complete, Cancel && cancel) {
		async::Deferred<T> success, fail;

		success.context(context, complete);
		fail.context(context, cancel);

		success.complete(future);
		fail.complete(async::cancel(future));
	}
}

class Cancelable
{
public:
	virtual ~Cancelable() {}

	virtual void cancel() = 0;
};

template<class ... A>
class Executable : public Cancelable
{
public:
	virtual ~Executable() {}

	virtual void execute(A &&... args) = 0;
};

template<>
class Executable<void> : public Cancelable
{
public:
	virtual ~Executable() {}

	virtual void execute() = 0;
};

template<>
class Executable<> : public Executable<void> {};

class AsyncChain : public QObject
{
	template<class T>
	friend class AsyncTask;

	Q_OBJECT
public:
	explicit AsyncChain() {
		_cancel.context(this, [this]() {
			_current->cancel();
		});
	}

	virtual ~AsyncChain() {}

	static inline AsyncTask<void()> * make();

	template<class F, class ... A, typename FR = decltype(std::declval<F>()(std::declval<A>()...))>
	static inline AsyncTask<remove_future_t<FR>()> * make(F && f, A &&... args);

	template<class T, class R, class ... A1, class ... A>
	static inline AsyncTask<R()> * make(T * object, QFuture<R>(T::*method)(A1...), A &&... args);

	AsyncChain * success_with(const std::function<void()> & onsuccess) {
		auto old = _succeeded;
		_succeeded = old ? [old, onsuccess]() {
			old();
			onsuccess();
		} : onsuccess;

		return this;
	}

	AsyncChain * fail_with(const std::function<void()> & onfail) {
		auto old = _failed;
		_failed = old ? [old, onfail]() {
			old();
			onfail();
		} :onfail;

		return this;
	}

	AsyncChain * end_with(const std::function<void()> & onend) {
		auto old = _ended;
		_ended = old ? [old, onend]() {
			old();
			onend();
		} : onend;

		return this;
	}

	AsyncChain * cancel_on(const QFuture<void> & future) {
		_cancel.complete(future);
		return this;
	}

	AsyncChain * cancel_on(const Observable<void> & o) {
		return cancel_on(o.future());
	}

	AsyncChain * subscribe(Deferred<void> & d) {
		return success_with([=]() mutable {
			d.complete();
		})
		-> fail_with([=]() mutable {
			d.cancel();
		});
	}

	template<class T, class F>
	AsyncChain * cancel_on(T * object, F member) {
		return cancel_on(async::observe(object, member));
	}

	void run() {
		_first->execute();
	}

private:
	void succeeded() {
		if(_succeeded) {
			_succeeded();
		}
	}

	void failed() {
		if(_failed) {
			_failed();
		}
	}

	void ended() {
		if(_ended) {
			_ended();
		}

		delete this;
	}

	Deferred<void> _cancel;
	Executable<> * _first = nullptr;
	Cancelable * _current = nullptr;
	std::function<void()> _succeeded = nullptr, _failed = nullptr, _ended = nullptr;
};

template<class R, class ... A>
class AsyncTask<R(A...)> : public Executable<A...>
{
	using Job = std::function<QFuture<R>(A &&...)>;
	using Result = return_to_arg_t<R>;
	using Then = std::function<func_t<void, Result>>;

public:
	AsyncTask(AsyncChain * chain, const Job & f) : _chain(chain), f(f) {}
	AsyncTask(AsyncChain * chain, Job && f) : _chain(chain), f(std::forward<Job>(f)) {}
	virtual ~AsyncTask() {}

	virtual void execute(A &&... args) override final {
		auto future = f(std::forward<A>(args)...);

		if(future.isFinished()) {
			succeed(future);
		} else if(future.isCanceled()) {
			cancel();
		} else {
			async::subscribe(_chain, future, [=]() {
				succeed(future);
			}, [=]() {
				cancel();
			});
		}
	}

	virtual void cancel() override final {
		if(_then) {
			_then->cancel();
		} else {
			_chain->failed();
			_chain->ended();
		}

		delete this;
	}

	template<typename Rslt = Result, typename = typename std::enable_if<std::is_same<Rslt, void>::value>::type>
	void succeed(const QFuture<R> &) {
		_chain->succeeded();

		if(_then) {
			_chain->_current = _then;
			_then->execute();
		} else {
			_chain->ended();
		}

		delete this;
	}

	template<typename Rslt = Result, typename = typename std::enable_if<!std::is_same<Rslt, void>::value>::type, int = 0>
	void succeed(const QFuture<R> & future) {
		_chain->succeeded();

		if(_then) {
			_chain->_current = _then;
			_then->execute(future);
		} else {
			_chain->ended();
		}

		delete this;
	}

	template<class F, class ... X, typename FR = decltype(std::declval<F>()(std::declval<X>()...)), typename = typename std::enable_if<std::is_same<Result, void>::value>::type>
	AsyncTask<func_t<remove_future_t<FR>, Result>> * then(const F & f, X && ... args) {
		static_assert(is_future<FR>::value, "Return value of 'f' must be QFuture");
		auto * task = new AsyncTask<func_t<remove_future_t<FR>, Result>>(_chain, std::bind(f, std::forward<X>(args)...));
		_then = task;
		return task;
	}

	template<class F, class ... X, typename FR = decltype(std::declval<F>()(std::declval<Result>(), std::declval<X>()...))>
	AsyncTask<func_t<remove_future_t<FR>, Result>> * then(const F & f, X && ... args) {
		static_assert(is_future<FR>::value, "Return value of 'f' must be QFuture");
		auto * task = new AsyncTask<func_t<remove_future_t<FR>, Result>>(_chain, std::bind(f, std::placeholders::_1, std::forward<X>(args)...));
		_then = task;
		return task;
	}

	template<class F, class ... X, class ... X1, typename FR, typename Rslt = Result,
		typename = typename std::enable_if<std::is_same<Rslt, void>::value>::type,
		typename = typename std::enable_if<std::is_same<std::tuple<typename std::decay<X>::type...>, std::tuple<typename std::decay<X1>::type...>>::value>::type
		>
	AsyncTask<func_t<remove_future_t<FR>, Result>> * then(F * object, QFuture<FR>(F::*method)(X1...), X && ... args) {
		auto * task = new AsyncTask<func_t<FR, Result>>(_chain, std::bind(method, object, std::forward<X>(args)...));
		_then = task;
		return task;
	}

	template<class F, class ... X, class ... X1, typename FR, typename Rslt = Result,
		typename = typename std::enable_if<!std::is_same<Rslt, void>::value>::type,
		typename = typename std::enable_if<std::is_same<std::tuple<typename std::decay<Rslt>::type, typename std::decay<X>::type...>, std::tuple<typename std::decay<X1>::type...>>::value>::type,
		int = 0
		>
	AsyncTask<func_t<remove_future_t<FR>, Result>> * then(F * object, QFuture<FR>(F::*method)(X1...), X && ... args) {
		auto * task = new AsyncTask<func_t<remove_future_t<FR>, Result>>(_chain, std::bind(method, object, std::placeholders::_1, std::forward<X>(args)...));
		_then = task;
		return task;
	}

	template<class T, typename Rslt = Result,
		typename = typename std::enable_if<std::is_same<Rslt, void>::value>::type
	>
	AsyncTask<func_t<T, Result>> * then(const Observable<T> & o) {
		auto * task = new AsyncTask<func_t<T, Result>>(_chain, [o](){
			return o.future();
		});

		_then = task;
		return task;
	}

	AsyncChain * chain() {
		return _chain;
	}

	AsyncChain * success_with(const std::function<void()> & onsuccess) {
		return _chain->success_with(onsuccess);
	}

	AsyncChain * fail_with(const std::function<void()> & onfail) {
		return _chain->fail_with(onfail);
	}

	AsyncChain * end_with(const std::function<void()> & onend) {
		return _chain->end_with(onend);
	}

	AsyncChain * cancel_on(const Observable<void> & o) {
		return _chain->cancel_on(o);
	}

	AsyncChain * cancel_on(const QFuture<void> & f) {
		return _chain->cancel_on(f);
	}

	template<class T, class F>
	AsyncChain * cancel_on(T * object, F member) {
		return _chain->cancel_on(async::observe(object, member));
	}

	AsyncChain * subscribe(Deferred<void> & d) {
		return _chain->subscribe(d);
	}

	void run() {
		_chain->run();
	}

private:
	AsyncChain * _chain;
	Job f;
	Executable<Result> * _then = nullptr;
};

namespace AsyncFuture {
	inline AsyncTask<void()> * chain() {
		return AsyncChain::make();
	}

	template<class F, class ... A, typename FR = decltype(std::declval<F>()(std::declval<A>()...))>
	inline AsyncTask<remove_future_t<FR>()> * chain(F && f, A &&... args) {
		return AsyncChain::make(std::forward<F>(f), std::forward<A>(args)...);
	}

	template<class T, class R, class ... A1, class ... A>
	inline AsyncTask<R()> * chain(T * object, QFuture<R>(T::*method)(A1...), A &&... args) {
		return AsyncChain::make(object, method, std::forward<A>(args)...);
	}
}

inline AsyncTask<void()> * AsyncChain::make() {
	auto * chain = new AsyncChain;
	auto * task = new AsyncTask<void()>(chain, (QFuture<void>(*)())async::complete);
	chain->_current = chain->_first = task;
	return task;
}

template<class F, class ... A, typename FR>
inline AsyncTask<remove_future_t<FR>()> * AsyncChain::make(F && f, A &&... args) {
	auto * chain = new AsyncChain;
	auto * task = new AsyncTask<remove_future_t<FR>()>(chain, std::bind(std::forward<F>(f), std::forward<A>(args)...));
	chain->_current = chain->_first = task;
	return task;
}

template<class T, class R, class ... A1, class ... A>
inline AsyncTask<R()> * AsyncChain::make(T * object, QFuture<R>(T::*method)(A1...), A &&... args) {
	auto * chain = new AsyncChain;
	auto * task = new AsyncTask<R()>(chain, std::bind(method, object, std::forward<A>(args)...));
	chain->_current = chain->_first = task;
	return task;
}

#endif // TASK_QUEUE_H
