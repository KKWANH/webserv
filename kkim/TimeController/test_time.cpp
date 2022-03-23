#include <time.h>
#include <sys/time.h>
#include <map>
#include <iostream>
//밀리초를 기준으로 한다

class Timer
{
private:
	clock_t start;
	std::map<int, clock_t> map;

public:
	void init_timer(int i)
	{
		map[i] = clock();
	}

	double get_time(int i)
	{
		clock_t end;
		end = clock();
		return (double(end - map[i]));
	}

	void del_time(int i)
	{
		map.erase(i);
	}

	void clean_time(int i)
	{
		map[i] = 0;
	}
};

int main(void)
{
	Timer t;

	t.init_timer(1);
	std::cout << t.get_time(1) << std::endl;
	t.clean_time(1);
	t.del_time(1);
	t.init_timer(2);
	std::cout << t.get_time(2) << std::endl;
}
