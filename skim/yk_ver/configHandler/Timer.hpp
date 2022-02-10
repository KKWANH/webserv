#ifndef TIMER_HPP
# define TIMER_HPP

#include <vector>
#include <sys/time.h>

class Timer {
	private:
		typedef struct s_Pair {
			void	*obj;
			long	genTime;
			long	expTime;
		} Pair;
		std::vector<Pair> _timeList;
	public:
		void	addObj(void *obj, int lifeTime)
		{
			struct timeval	tv;
			Pair			pair;

			gettimeofday(&tv, NULL);
			pair.obj = obj;
			pair.genTime = tv.tv_sec;
			pair.expTime = tv.tv_sec + long(lifeTime);
			_timeList.push_back(pair);
		}

		void	resetObj(void *obj, int lifeTime)
		{
			struct timeval	tv;
			std::vector<Pair>::iterator	it;
			gettimeofday(&tv, NULL);
			for (it = _timeList.begin(); it != _timeList.end(); it++)
			{
				if (it->obj == obj)
				{
					it->genTime = tv.tv_sec;
					it->expTime = tv.tv_sec + long(lifeTime);
					break ;
				}
			}
		}

		void	delObj(void *obj, void (*del)(void *))
		{
			std::vector<Pair>::iterator	it;
			for (it = _timeList.begin(); it != _timeList.end(); it++)
			{
				if (it->obj == obj)
				{
					del(it->obj);
					_timeList.erase(it);
					break ;
				}
			}
		}

		void	checkTime(void (*del)(void *))
		{
			struct timeval	tv;
			gettimeofday(&tv, NULL);
			std::vector<Pair>::iterator	it;
			for (it = _timeList.begin(); it != _timeList.end(); it++)
			{
				if (it->expTime <= tv.tv_sec)
				{
					del(it->obj);
					_timeList.erase(it);
					break ;
				}
			}
		}
};

#endif
