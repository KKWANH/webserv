#include <ctime>
#include <sys/time.h>
#include <map>
#include <iostream>
#include <unistd.h>
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

	time_t start = time(NULL);
	for (int i = 0; i < 10; i++)
	{
		time_t end = time(NULL);
		std::cout << "start : " << start << " end : " << end << std::endl;
		std::cout << "time : " << double(end - start)<< std::endl;
		sleep(1);
	}
	return (0);
}
