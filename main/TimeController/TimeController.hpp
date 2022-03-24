#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <ctime>
#include <sys/time.h>
#include <map>
#include <iostream>
#include <string>

class TimeController
{
private:
	struct timer_info
	{
		void 	*obj;
		time_t start;
		int		keepalive_timeout;

//		timer_info(HTTPConnection &ob) : obj(ob) { }
	};
	std::map<int, timer_info> timer_list;

public:

	void init_time(int id, void *ob, int timeout)
	{
		timer_info info;

		info.obj = ob;
		info.start = time(NULL);
		info.keepalive_timeout = timeout;
		timer_list[id] = info;
		//timer_list[id] = clock();
//		std::cout << "timer start : " << (double)timer_list[id] << std::endl;
	}

	double get_time(int id)
	{
		time_t end;
		end = time(NULL);
//		std::cout << "start : " << timer_list[id].start << " end : " << double(end) << std::endl;
//		std::cout << "time :  " << double(end - timer_list[id].start) << std::endl;
		return (double(end - timer_list[id].start));
	}

	void del_time(int id)
	{
		timer_list.erase(id);
		std::cout << "timer delete : " << id << std::endl;
	}

	void clean_time(int id)
	{
		if (timer_list.find(id) != timer_list.end())
			timer_list[id].start = time(NULL);
		else
			return ;
//		std::cout << id << " timer reset : " << timer_list[id].start << std::endl;
	}

	bool find_time(int id)
	{
		if (timer_list.find(id) != timer_list.end())
			return (true);
		else
			return (false);
	}

	//디폴트가 60초
	void check_time(void (*del)(void *))
	{
		int flag = 0;
		std::map<int, timer_info>::iterator del_it;
		//(void)(del);
		for (std::map<int, timer_info>::iterator it = timer_list.begin(); it != timer_list.end(); it++)
		{
			if (flag == 1)
			{	
				del(del_it->second.obj);
				timer_list.erase(del_it->first);
				std::cout << "keepalive_timeout bye" << std::endl; 
				flag = 0;
			}
			if (get_time(it->first) >= it->second.keepalive_timeout) {
				flag = 1;
				del_it = it;
			}
		}
		if (flag == 1)
		{
			del(del_it->second.obj);
			timer_list.erase(del_it->first);
			std::cout << "keepalive_timeout bye" << std::endl; 
		}
	}
};


#endif
