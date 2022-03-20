#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <time.h>
#include <sys/time.h>
#include <map>
#include <iostream>

class TimeController
{
private:
	std::map<void*, clock_t> timer_list;

public:

	void init_time(void* id)
	{
		timer_list[id] = clock();
		std::cout << "timer start : " << (double)timer_list[id] << std::endl;
	}

	double get_time(void *id)
	{
		clock_t end;
		end = clock();
	//	std::cout << i << " end :" << (double)end << std::endl;
	//	std::cout << i << " start :" << (double)timer_list[i] << std::endl;
//		std::cout << "time : " << (double)(end - timer_list[i]) << std::endl;
		return (double(end - timer_list[id]));
	}

	void del_time(void *id)
	{
		timer_list.erase(id);
		std::cout << "timer delete" << std::endl;
	}

	void clean_time(void* id)
	{
		if (timer_list.find(id) != timer_list.end())
			timer_list[id] = clock();
		else
			return ;
		std::cout << "timer reset" << std::endl;
	}

	bool find_time(void* id)
	{
		if (timer_list.find(id) != timer_list.end())
			return (true);
		else
			return (false);
	}
/*
	void check_time(void* id, int block)
	{
		int keepalive_timeout;

		std::map<void*, clock_t>::iterator it;
		it = timer_list.find(id);
		keepalive_timeout = itoa(_config._http._server[block]._dir_map["keepalive_timeout"])
		if (keepalive_timeout < 0)
	//	std::cout << "Asd" << std::endl;//	std::cout << "keepalivetimeout1 : " << _config._http._dir_map["keepalive_timeout"] << std::endl;
	//	std::cout << "keepalivetimeout2 : " << _config._http._server[0]._dir_map["keepalive_timeout"] << std::endl;
	//	std::cout << "keepalivetimeout2 : " << _config._http._server[1]._dir_map["keepalive_timeout"] << std::endl;
	}
	*/
};


#endif
