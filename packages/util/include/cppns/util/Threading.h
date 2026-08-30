#pragma once

#include <condition_variable>
#include <mutex>
#include <functional>
#include <thread>
#include <deque>

#include "cppns/memory/Memory.h"

// The worker is designed to work with threads to complete tasks, by itself it is not multithreaded but is thread safe
class CWorker {

	std::mutex m_QueueMutex;

	std::condition_variable m_QueueCV;

	std::condition_variable m_FinishedCV;

	std::deque<std::function<void()>> m_Tasks;

	bool m_Stop = false;

public:

	CWorker() = default;

	CWorker(const CWorker& inWorker): m_Tasks(inWorker.m_Tasks), m_Stop(inWorker.m_Stop) {}

	CWorker& operator=(const CWorker& inWorker) {
		m_Tasks = inWorker.m_Tasks;
		m_Stop = inWorker.m_Stop;
		return *this;
	}

	bool execute() {
		std::function<void()> task;
		{
			std::unique_lock lock(m_QueueMutex);
			m_QueueCV.wait(lock, [this] {
				return !m_Tasks.empty() || m_Stop;
			});

			if (m_Stop && m_Tasks.empty()) return true;

			// Get task and remove from tasks
			task = m_Tasks.front();
			m_Tasks.pop_front();
		}

		task();

		if (m_Tasks.empty()) {
			m_FinishedCV.notify_all();
		}

		return false;
	}

	void flush() {
		while (getNumberOfTasks() > 0) {
			std::function<void()> task;
			{
				std::unique_lock lock(m_QueueMutex);

				// Get task and remove from tasks
				task = m_Tasks.front();
				m_Tasks.pop_front();
			}

			task();

			if (m_Tasks.empty()) {
				m_FinishedCV.notify_all();
			}
		}
	}

	[[nodiscard]] size_t getNumberOfTasks() const {
		return m_Tasks.size();
	}

	[[nodiscard]] bool isWorkerRunning() const {
		return !m_Tasks.empty();
	}

	void stop() {
		{
			std::lock_guard lock(m_QueueMutex);
			m_Tasks.clear();
			m_Stop = true;
		}
		m_QueueCV.notify_all();
	}

	void add(const std::function<void()>& inFunc) {
		{
			std::lock_guard lock(m_QueueMutex);
			m_Tasks.push_back(inFunc);
		}
		m_QueueCV.notify_all();
	}

	void addNext(const std::function<void()>& inFunc) {
		{
			std::lock_guard lock(m_QueueMutex);
			m_Tasks.push_front(inFunc);
		}
		m_QueueCV.notify_all();
	}

	void wait() {
		std::unique_lock lock(m_QueueMutex);

		// Tells the worker to wait until it gets a signal and there are no tasks left, can wait indefinitely if tasks are looped
		m_FinishedCV.wait(lock, [this] {return m_Tasks.empty();});
	}
};

// A generic thread, useful for a single multithreaded task (like a file import).  Can also take in a worker and will do all of it's tasks and then finish.
class CThread {

public:

	static size_t totalThreads() { return std::thread::hardware_concurrency(); }

	explicit CThread(const std::function<void()>& func): m_Thread(func) {}

	explicit CThread(CWorker& inWorker): m_Thread([&inWorker] {
		while (inWorker.isWorkerRunning()) {
			if (inWorker.execute()) return;
		}
	}) {}

	~CThread() {
		stop();
	}

	void stop() {
		if (m_Thread.joinable()) m_Thread.join();
	}

private:

	std::thread m_Thread;

};

// A persistent thread, this thread is designed to complete tasks until told to stop.  If no tasks are present it blocks its own thread
class CPersistentThread {

public:

	explicit CPersistentThread(CWorker& inWorker): m_Worker(inWorker), m_Thread([this] {
		while (true) {
			if (m_Worker.execute()) return;
		}
	}) {}

	~CPersistentThread() {
		stop();
	}

	void wait() const {
		m_Worker.wait();
	}

	void stop() {
		m_Worker.stop();
		wait();
		if (m_Thread.joinable()) m_Thread.join();
	}

	void sleep(size_t inMilliseconds) const {
		m_Worker.addNext([inMilliseconds] {
			std::this_thread::sleep_for(std::chrono::milliseconds(inMilliseconds));
		});
	}

private:

	CWorker& m_Worker;
	std::thread m_Thread;

};

// Designed to handle multiple threads simultaneously that all compete for a group of tasks.  If many tasks are given, each thread will work on one until all tasks are finished.
class CThreadPool {

public:

	explicit CThreadPool(CWorker& inWorker, const uint32_t inNumThreads = CThread::totalThreads()): m_Worker(inWorker) {
		for (uint32_t i = 0; i < inNumThreads; ++i) {
			m_Threads.emplace_back([this] {
				while (true) {
					if (m_Worker.execute()) return;
				}
			});
		}
	}



	~CThreadPool() {
		stop();
	}

	[[nodiscard]] CWorker& getWorker() { return m_Worker; }

	[[nodiscard]] const CWorker& getWorker() const { return m_Worker; }

	void wait() const {
		m_Worker.wait();
	}

	void stop() {
		m_Worker.stop();
		wait();
		for (auto& thread : m_Threads) {
			if (thread.joinable()) thread.join();
		}
	}

	// TODO: not a smart method, need thread tracking.
	void sleep(size_t inMilliseconds) const {
		for (size_t i = 0; i < m_Threads.size(); ++i) {
			m_Worker.addNext([inMilliseconds] {
				std::this_thread::sleep_for(std::chrono::milliseconds(inMilliseconds));
			});
		}
	}

private:

	CWorker& m_Worker;
	std::vector<std::thread> m_Threads{};

};

// Thanks to Jonathan Wakely on Stack Exchange.
// https://stackoverflow.com/questions/16859519/how-to-wrap-calls-of-every-member-function-of-a-class-in-c11
template <typename TType>
class TThreadSafe {
	template<typename TParent, typename TMutex>
	struct safe_lock : std::lock_guard<TMutex> {

		explicit safe_lock(TParent* parent, TMutex& mtx) noexcept(false)
		: std::lock_guard<TMutex>(mtx),
		  parent(parent) {}

		decltype(auto) operator->() const noexcept { return parent; }

		TParent* parent;
	};

	template<typename TParent, typename TMutex>
	safe_lock(TParent* parent, TMutex& mtx) -> safe_lock<TParent, TMutex>;

public:

	/*
	 * Mutex cannot be copied nor moved, so we ignore it
	 */

	TThreadSafe() noexcept {
		if constexpr (sstl::is_initializable_v<TType>) {
			m_obj.init();
		}
	}

	template <typename... TArgs,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_null_pointer<std::decay_t<TArgs>>>...,
				std::negation<std::is_same<std::decay_t<TArgs>, TThreadSafe>>...
			>,
			int> = 0
	>
	explicit TThreadSafe(TArgs&&... args) noexcept {
		m_obj = TType{std::forward<TArgs>(args)...};
		if constexpr (sstl::is_initializable_v<TType>) {
			m_obj.init();
		}
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe(const TThreadSafe<TOtherType>& otr) noexcept
	: TThreadSafe(otr.m_obj) {}

	TThreadSafe(const TThreadSafe& otr) noexcept
	: TThreadSafe(otr.m_obj) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe(TThreadSafe<TOtherType>& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: TThreadSafe(otr.m_obj) {}

	TThreadSafe(TThreadSafe& otr) noexcept
	: TThreadSafe(otr.m_obj) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe(TThreadSafe<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>)
#else
	noexcept
#endif
	: TThreadSafe(std::move(otr.m_obj)) {}

	TThreadSafe(TThreadSafe&& otr) noexcept
	: TThreadSafe(std::move(otr.m_obj)) {}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe& operator=(const TThreadSafe<TOtherType>& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		this->m_obj = otr.m_obj;
		return *this;
	}

	TThreadSafe& operator=(const TThreadSafe& otr) noexcept {
		this->m_obj = otr.m_obj;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe& operator=(TThreadSafe<TOtherType>& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		this->m_obj = otr.m_obj;
		return *this;
	}

	TThreadSafe& operator=(TThreadSafe& otr) noexcept {
		this->m_obj = otr.m_obj;
		return *this;
	}

	template <typename TOtherType = TType,
		std::enable_if_t<std::is_convertible_v<TOtherType*, TType*>, int> = 0
	>
	TThreadSafe& operator=(TThreadSafe<TOtherType>&& otr)
#ifdef __cpp_lib_is_nothrow_convertible
	noexcept(std::is_nothrow_convertible_v<TOtherType*, TType*>) {
#else
	noexcept {
#endif
		this->m_obj = std::move(otr.m_obj);
		return *this;
	}

	TThreadSafe& operator=(TThreadSafe&& otr) noexcept {
		this->m_obj = std::move(otr.m_obj);
		return *this;
	}

	void lockFor(const std::function<void(TType&)>& func) noexcept(false) {
		std::lock_guard lock(mtx);
		func(m_obj);
	}

	decltype(auto) operator->() noexcept(false) {
		if constexpr (TUnfurled<std::remove_reference_t<TType>>::isManaged) {
			return safe_lock(m_obj.get(), mtx);
		} else {
			return safe_lock(&m_obj, mtx);
		}
	}

	decltype(auto) operator->() const noexcept(false) {
		if constexpr (TUnfurled<std::remove_reference_t<TType>>::isManaged) {
			return safe_lock(m_obj.get(), mtx);
		} else {
			return safe_lock(&m_obj, mtx);
		}
	}

	friend bool operator<(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj < snd.m_obj;
	}

	friend bool operator<=(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj <= snd.m_obj;
	}

	friend bool operator>(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj > snd.m_obj;
	}

	friend bool operator>=(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj >= snd.m_obj;
	}

	friend bool operator==(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj == snd.m_obj;
	}

	friend bool operator!=(const TThreadSafe& fst, const TThreadSafe& snd) noexcept {
		return fst.m_obj != snd.m_obj;
	}

	friend size_t getHash(const TThreadSafe& obj) noexcept {
		return getHash(static_cast<const TType&>(obj));
	}

private:
	template <typename>
	friend class TThreadSafe;

	TType m_obj;

	mutable std::recursive_mutex mtx;
};