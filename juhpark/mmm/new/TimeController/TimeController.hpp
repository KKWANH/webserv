#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <time.h>
#include <sys/time.h>
#include <map>
#include <iostream>
#include <string>

class TimeController
{
private:
	std::map<int, clock_t> timer_list;

public:

	void init_time(int id)
	{
		timer_list[id] = clock();
		std::cout << "timer start : " << (double)timer_list[id] << std::endl;
	}

	double get_time(int id)
	{
		clock_t end;
		end = clock();
		return (double(end - timer_list[id]));
	}

	void del_time(int id)
	{
		timer_list.erase(id);
		std::cout << "timer delete" << std::endl;
	}

	void clean_time(int id)
	{
		if (timer_list.find(id) != timer_list.end())
			timer_list[id] = clock();
		else
			return ;
//		std::cout << "timer reset" << std::endl;
	}

	bool find_time(int id)
	{
		if (timer_list.find(id) != timer_list.end())
			return (true);
		else
			return (false);
	}

	bool check_time(int id, int block)
	{
		int keepalive_timeout = 0;

		std::map<int, clock_t>::iterator it;
		it = timer_list.find(id);
		keepalive_timeout = atoi(_config._http._server[block]._dir_map["keepalive_timeout"].c_str());
		if (keepalive_timeout < 0) {
			keepalive_timeout = atoi(_config._http._dir_map["keepalive_timeout"].c_str());
			if (keepalive_timeout < 0)
				return (true);
		}
		if (get_time(id)/CLOCKS_PER_SEC >= keepalive_timeout)
			return (true);
		return (false);
		

	//	std::cout << "Asd" << std::endl;//	std::cout << "keepalivetimeout1 : " << _config._http._dir_map["keepalive_timeout"] << std::endl;
	//	std::cout << "keepalivetimeout2 : " << _config._http._server[0]._dir_map["keepalive_timeout"] << std::endl;
	//	std::cout << "keepalivetimeout2 : " << _config._http._server[1]._dir_map["keepalive_timeout"] << std::endl;
	}
};


#endif
