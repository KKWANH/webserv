#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <time.h>
#include <sys/time.h>
#include <map>
#include <iostream>

class TimeController
{
private:
	std::map<int, clock_t> timer_list;

public:
	void init_time(int i)
	{
		timer_list[i] = clock();
		std::cout << i << " : timer start : " << (double)timer_list[i] << std::endl;
	}

	double get_time(int i)
	{
		clock_t end;
		end = clock();
	//	std::cout << i << " end :" << (double)end << std::endl;
	//	std::cout << i << " start :" << (double)timer_list[i] << std::endl;
//		std::cout << "time : " << (double)(end - timer_list[i]) << std::endl;
		return (double(end - timer_list[i]));
	}

	void del_time(int i)
	{
		timer_list.erase(i);
		std::cout << i << " : timer delete" << std::endl;
	}

	void clean_time(int i)
	{
		if (timer_list.find(i) != timer_list.end())
			timer_list[i] = clock();
		else
			return ;
		std::cout << i << " : timer reset" << std::endl;
	}

	bool find_time(int i)
	{
		if (timer_list.find(i) != timer_list.end())
			return (true);
		else
			return (false);
	}
};


#endif
