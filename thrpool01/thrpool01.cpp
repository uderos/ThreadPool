// thrpool01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "thread_pool.h"
#include "dbg_utils.h"

static void f_thread_fn_01(const int n)
{
	DBG_OUT << __FUNCTION__
		<< "() tid=" << std::this_thread::get_id()
		<< " n=" << n;
}

static void f_test00()
{
	DBG_OUT << "A=" << 1 << " B=" << 2;
	DBG_OUT << "X=" << 11 << " Y=" << 22;
}

static void f_test01()
{
	using data_ptr_t = std::unique_ptr<int>;
	udr::thread_pool<decltype(f_thread_fn_01), int> tp(f_thread_fn_01, 4);

	tp.execute(std::make_shared<int>(1));
	tp.execute(std::make_shared<int>(2));
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	tp.terminate_and_wait();
}


int main()
{
	f_test00();
	f_test01();

    return 0;
}

