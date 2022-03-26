// NOTE: modified coding convention by joopark
#include "TimeController.hpp"

void TimeController::init_time(int id, void *ob, int timeout) {
	timer_info info;

	info.obj = ob;
	info.start = time(NULL);
	info.keepalive_timeout = timeout;
	timer_list[id] = info;
}

double TimeController::get_time(int id) {
	time_t end;
	end = time(NULL);
	// std::cout << "start : " << timer_list[id].start << " end : " << double(end) << std::endl;
	// std::cout << "time :  " << double(end - timer_list[id].start) << std::endl;
	return (double(end - timer_list[id].start));
}

void TimeController::del_time(int id) {
	timer_list.erase(id);
	std::cout << "timer delete : " << id << std::endl;
}

void TimeController::clean_time(int id) {
	if (timer_list.find(id) != timer_list.end())
		timer_list[id].start = time(NULL);
	else
		return ;
//		std::cout << id << " timer reset : " << timer_list[id].start << std::endl;
}

bool TimeController::find_time(int id) {
	if (timer_list.find(id) != timer_list.end())
		return (true);
	else
		return (false);
}

void TimeController::check_time(void (*del)(void *)) {
	int flag = 0;
	std::map<int, timer_info>::iterator del_it;
	std::map<int, timer_info>::iterator iter;
	for (iter = timer_list.begin(); iter != timer_list.end(); iter++) {
		if (flag == 1) {	
			del(del_it->second.obj);
			timer_list.erase(del_it->first);
			std::cout << "keepalive_timeout bye" << std::endl; 
			flag = 0;
		}
		if (get_time(iter->first) >= iter->second.keepalive_timeout) {
			flag = 1;
			del_it = iter;
		}
	}
	if (flag == 1) {
		del(del_it->second.obj);
		timer_list.erase(del_it->first);
		std::cout << "keepalive_timeout bye" << std::endl; 
	}
}