#include "stdafx.h"
#include "dbg_utils.h"

std::mutex dbg_utils::m_mutex;

dbg_utils::dbg_utils()
{
}

dbg_utils::~dbg_utils()
{
	const std::string msg = m_oss.str();
	if (!msg.empty())
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		std::cout << "Thread_" << std::this_thread::get_id()
			<< ": " << msg
			<< std::endl;
	}
}

//void dbg_utils::log(const std::string & msg) const
//{
//	std::unique_lock<std::mutex> lock(m_mutex);
//	std::cout << "Thread_" << std::this_thread::get_id()
//		<< ": " << msg
//		<< std::endl;
//}

