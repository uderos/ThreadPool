#include "stdafx.h"
#include "thread_pool.h"
#include "dbg_utils.h"

struct work_data_t
{
	using clock_t = std::chrono::high_resolution_clock;
	clock_t::time_point t1;
	clock_t::time_point t2;
	clock_t::time_point t3;
};

static void f_thread_fn_01(work_data_t & work_data)
{
	work_data.t3 = work_data_t::clock_t::now();

	volatile int x = 0;
	while (x < 10000)
		++x;
}

template <typename TIME_POINT_T>
static std::size_t f_calc_ms(const TIME_POINT_T & from_time,
							 const TIME_POINT_T & to_time)
{
	const auto d = (to_time - from_time);
	const auto d_ms = std::chrono::duration_cast<std::chrono::milliseconds>(d);
	return static_cast<std::size_t>(d_ms.count());
}

template <typename TIME_POINT_T>
static std::size_t f_calc_us(const TIME_POINT_T & from_time,
	const TIME_POINT_T & to_time)
{
	const auto d = (to_time - from_time);
	const auto d_ms = std::chrono::duration_cast<std::chrono::microseconds>(d);
	return static_cast<std::size_t>(d_ms.count());
}

void g_time_test01()
{
	std::cout << "\n\n" << __FUNCTION__ << " BEGIN" << std::endl;

	static constexpr std::size_t NUM_THREADS = 8;
	static constexpr std::size_t NUM_TASKS{ 1000 };

	std::vector<std::shared_ptr<work_data_t>> work_data_list;
	for (int i = 0; i < NUM_TASKS; ++i)
		work_data_list.push_back(std::make_shared<work_data_t>());

	udr::thread_pool<decltype(f_thread_fn_01), work_data_t> tp(f_thread_fn_01, NUM_THREADS);

	DBG_OUT << "Sending tasks ...";
	const auto start_time = work_data_t::clock_t::now();


	for (auto wd : work_data_list)
	{
		wd->t1 = work_data_t::clock_t::now();
		tp.execute(wd);
		wd->t2 = work_data_t::clock_t::now();
	}

	DBG_OUT << "Waiting for completion ...";

	while (tp.has_pending_tasks())
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	DBG_OUT << "Terminating the pool";
	tp.terminate_and_wait();

	for (auto wd : work_data_list)
	{
		const auto d1 = f_calc_us(start_time, wd->t1);
		const auto d2 = f_calc_us(start_time, wd->t2);
		const auto d3 = f_calc_us(start_time, wd->t3);
		const auto d23 = d3 - d2;
		DBG_OUT << "d1=" << d1
				<< " d2=" << d2
				<< " d3=" << d3
				<< " d23=" << d23
				<< " usecs";
	}

}

