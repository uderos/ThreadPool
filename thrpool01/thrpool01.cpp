// thrpool01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "thread_pool.h"

static void f_thread_fn_01(const int n)
{
	std::cout << __FUNCTION__
		<< "() tid=" << std::this_thread::get_id()
		<< " n=" << n
		<< std::endl;
}

static void f_test01()
{
	using data_ptr_t = std::unique_ptr<int>;
	udr::thread_pool<decltype(f_thread_fn_01), int> tp(f_thread_fn_01, 4);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	tp.execute(std::make_shared<int>(1));
	tp.execute(std::make_shared<int>(2));
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	tp.terminate_and_wait();
}


int main()
{
	f_test01();

    return 0;
}

