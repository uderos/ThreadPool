// thrpool01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "thread_pool.h"
#include "dbg_utils.h"


static int f_counter = 0;

static void f_thread_fn_01(const int n)
{
	DBG_OUT << __FUNCTION__
		<< "() tid=" << std::this_thread::get_id()
		<< " n=" << n;
}

static void f_thread_fn_02(const int n)
{
	++f_counter;
	DBG_OUT << __FUNCTION__
		<< "() tid=" << std::this_thread::get_id()
		<< " n=" << n
		<< " cnt=" << f_counter;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	DBG_OUT << __FUNCTION__
		<< "() tid=" << std::this_thread::get_id()
		<< " END";
}

static void f_test00()
{
	std::cout << "\n\n" << __FUNCTION__ << " BEGIN" << std::endl;
	DBG_OUT << "A=" << 1 << " B=" << 2;
	DBG_OUT << "X=" << 11 << " Y=" << 22;
}

static void f_test01()
{
	std::cout << "\n\n" << __FUNCTION__ << " BEGIN" << std::endl;
	udr::thread_pool<decltype(f_thread_fn_01), int> tp(f_thread_fn_01, 4);

	tp.execute(std::make_shared<int>(1));
	tp.execute(std::make_shared<int>(2));
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	tp.terminate_and_wait();
}

static void f_test02()
{
	std::cout << "\n\n" << __FUNCTION__ << " BEGIN" << std::endl;
	udr::thread_pool<decltype(f_thread_fn_01), int> tp(f_thread_fn_01, 4);

	for (int i = 0; i < 1000; ++i)
		tp.execute(std::make_shared<int>(i));

	DBG_OUT << "Waiting for completion...";

	while (tp.has_pending_tasks())
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	DBG_OUT << "Terminating the pool";
	tp.terminate_and_wait();
}

static void f_test03()
{
	std::cout << "\n\n" << __FUNCTION__ << " BEGIN" << std::endl;
	udr::thread_pool<decltype(f_thread_fn_02), int> tp(f_thread_fn_02, 4);

	const int NUM_RUNS = 19;
	f_counter = 0;
	for (int i = 0; i < NUM_RUNS; ++i)
		tp.execute(std::make_shared<int>(i));

	while (f_counter < NUM_RUNS)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	tp.terminate_and_wait();
}

int main()
{
	extern void g_time_test01();

//	f_test00();
//	f_test01();
//	f_test02();
//	f_test03();
	g_time_test01();

    return 0;
}

