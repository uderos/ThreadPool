#ifndef UDR_THREAD_POOL_H
#define UDR_THREAD_POOL_H

namespace udr {

template <typename WORK_FN_T, typename WORK_DATA_T>
class thread_pool
{
	public:
		using data_ptr_t = std::shared_ptr<WORK_DATA_T>;

		thread_pool(WORK_FN_T & work_fn);
		thread_pool(WORK_FN_T & work_fn, 
					const std::size_t num_threads);
		thread_pool(const thread_pool &) = delete;
		thread_pool(thread_pool &&) = delete; // Maybe later
		virtual ~thread_pool();

		void execute(data_ptr_t data_ptr);

		void terminate_and_wait();
		void terminate_no_wait();

		// void dbg_dump(std::ostream & os) const;

	private:

		using thread_ptr_t = std::unique_ptr<std::thread>;
		using lock_t = std::unique_lock<std::mutex>;

		mutable std::condition_variable m_condition_variable;
		mutable std::mutex m_mutex;

		WORK_FN_T * m_work_fn;

		std::atomic_bool m_termination_flag;

		std::vector<thread_ptr_t> m_working_threads;
		std::queue<data_ptr_t> m_working_data_queue;

		void m_thread_fn(const int tid);
};

template <typename WORK_FN_T, typename WORK_DATA_T>
thread_pool<WORK_FN_T, WORK_DATA_T>::thread_pool(WORK_FN_T & work_fn) :
	thread_pool(work_fn, std::thread::hardware_concurrency())
{
}

template <typename WORK_FN_T, typename WORK_DATA_T>
thread_pool<WORK_FN_T, WORK_DATA_T>::thread_pool(WORK_FN_T & work_fn, 
												 const std::size_t num_threads) :
	m_work_fn(work_fn),
	m_termination_flag(false)
{
	for (std::size_t i = 0; i < num_threads; ++i)
		m_working_threads.push_back(
			std::make_unique<std::thread>(
				&thread_pool<WORK_FN_T, WORK_DATA_T>::m_thread_fn, this, i));
}

template <typename WORK_FN_T, typename WORK_DATA_T>
thread_pool<WORK_FN_T, WORK_DATA_T>::~thread_pool()
{
	//if (!m_termination_flag)
	//	throw std::runtime_error("thread_pool not terminated");
}

template <typename WORK_FN_T, typename WORK_DATA_T>
void thread_pool<WORK_FN_T, WORK_DATA_T>::execute(data_ptr_t data_ptr)
{
	m_working_data_queue.push(data_ptr);
	m_condition_variable.notify_one();
}

template <typename WORK_FN_T, typename WORK_DATA_T>
void thread_pool<WORK_FN_T, WORK_DATA_T>::terminate_and_wait()
{
	m_termination_flag = true;

	m_condition_variable.notify_all();

//	std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // UBEDEBUG


	for (auto & thr_ptr : m_working_threads)
	{
		if ((*thr_ptr).joinable())
			(*thr_ptr).join();
	}

	m_working_threads.clear();
}

template <typename WORK_FN_T, typename WORK_DATA_T>
void thread_pool<WORK_FN_T, WORK_DATA_T>::terminate_no_wait()
{
	for (auto & thr_ptr : m_working_threads)
		(*thr_ptr).detach();

	m_termination_flag = true;

	m_condition_variable.notify_all();
}

template <typename WORK_FN_T, typename WORK_DATA_T>
void thread_pool<WORK_FN_T, WORK_DATA_T>::m_thread_fn(const int tid)
{
	DBG_OUT << "Thread_" << std::this_thread::get_id() << " starts";

	auto cv_fn = [this] {
		const bool data_available = (!this->m_working_data_queue.empty());
		return data_available || m_termination_flag;
	};

	while (!m_termination_flag)
	{
		DBG_OUT << "Thread_" << std::this_thread::get_id() << " waits";
		lock_t lock(m_mutex);
		m_condition_variable.wait(lock, cv_fn);
		DBG_OUT << "Thread_" << std::this_thread::get_id()
			<< " wakes up"
			<< " tf=" << m_termination_flag
			<< " #q=" << m_working_data_queue.size();

		if ((!m_termination_flag) &&
			(!m_working_data_queue.empty()))
		{
			data_ptr_t data_ptr = m_working_data_queue.front();
			m_working_data_queue.pop();
			lock.unlock();
			m_work_fn(*data_ptr);
		}
	}
	DBG_OUT << "Thread_" << std::this_thread::get_id() << " terminates";
}



}; // namespace udr
#endif // UDR_THREAD_POOL_H
