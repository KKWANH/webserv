#ifndef KERNELQUEUECONTROLLER_HPP
# define KERNELQUEUECONTROLLER_HPP

#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <iostream>
#include <fcntl.h>

class KernelQueueController {
	int kfd;
	struct timespec pollingTime;
	size_t get_events_size;
	size_t set_events_size;
	struct kevent* getEvent;
	struct kevent* setEvent;
public:
	KernelQueueController(size_t events_size) : get_events_size(events_size), set_events_size(0) {
		// polling time = 1 sec
		pollingTime.tv_sec = 1;
		pollingTime.tv_nsec = 0;
		getEvent = new struct kevent[events_size];
		setEvent = new struct kevent[events_size];
		kfd = kqueue();
	}
	~KernelQueueController() {
		delete getEvent;
		delete setEvent;
		close(kfd);
	}
	size_t accessEvents() {
		size_t events = kevent(kfd, setEvent, set_events_size, getEvent, get_events_size, &pollingTime);
		set_events_size = 0;
		return events;
	}
	void addEvent(int fd, int16_t event, void* instancePointer) {
		EV_SET(&setEvent[set_events_size++], fd, event, EV_ADD | EV_EOF, 0, 0, instancePointer);
	}

	void removeEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_DELETE, 0, 0, instancePointer);
	}

	void enableEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_ENABLE, 0, 0, instancePointer);
	}

	void disableEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_DISABLE, 0, 0, instancePointer);
	}

	void* getInstanceByEventIndex(int index) {
		return getEvent[index].udata;
	}

	int getFdByEventIndex(int index) {
		return getEvent[index].ident;
	}

	bool isCloseByEventIndex(int index) {
    	return (getEvent[index].flags & EV_EOF);
	}
};

#endif
