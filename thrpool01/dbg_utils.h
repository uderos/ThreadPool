#pragma once

#define DBG_OUT (dbg_utils())

class dbg_utils
{
public:

	dbg_utils();
	~dbg_utils();
	dbg_utils(const dbg_utils &) = delete;
	dbg_utils(dbg_utils &&) = delete;

//	void log(const std::string & msg) const;

	template <typename T>
	dbg_utils & operator<<(const T & data);


private:

	static std::mutex m_mutex;
	std::ostringstream m_oss;
};

template <typename T>
dbg_utils & dbg_utils::operator<<(const T & data)
{
	m_oss << data;
	return (*this);
}

