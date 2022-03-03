#include <time.h>
#include <sys/time.h>
#include <vector>
#include <iostream>


class Timer
{
private:
	double result;
	clock_t start;
	clock_t end;

public:
	void init_timer(void)
	{
		start = clock();
	}

	double get_time(void)
	{
		end = clock();
		return (double(end - start));
	}

	void reset_time(void)
	{
		start = clock();
	}

	void clean_time(void)
	{
		start = 0;
		end = 0;
	}
};

int main(void)
{
	Timer t;

	t.init_timer();
	std::cout << t.get_time() << std::endl;
	t.clean_time();
	t.reset_time();
	std::cout << t.get_time() << std::endl;
}
