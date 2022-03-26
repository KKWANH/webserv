// NOTE: modified coding convention by joopark
#ifndef TIMECONTROLLER_HPP
# define TIMECONTROLLER_HPP

#include <ctime>
#include <sys/time.h>
#include <map>
#include <iostream>
#include <string>

class TimeController {
private:
	struct timer_info {
		void 	*obj;
		time_t start;
		int		keepalive_timeout;
	};
	std::map<int, timer_info> timer_list;

public:
	void	init_time(int id, void *ob, int timeout);
	double	get_time(int id);
	void	del_time(int id);
	void	clean_time(int id);
	bool	find_time(int id);
	void	check_time(void (*del)(void *));
};


#endif
