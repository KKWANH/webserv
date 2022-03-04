#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <time.h>
#include <sys/time.h>
#include <map>
#include <iostream>

class Timer
{
private:
	map<int, clock_t> timer_list;

public:
	void init_timer(int i)
	{
		timer_list[i] = clock();
	}

	double get_time(int i)
	{
		clock_t end;
		end = clock();
		return (double(end - timer_list[i]));
	}

	void del_time(int i)
	{
		timer_list.erase(i);
	}

	void clean_time(int i)
	{
		timer_list[i] = 0;
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
