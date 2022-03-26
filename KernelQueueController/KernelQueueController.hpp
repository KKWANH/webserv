#ifndef KERNELQUEUECONTROLLER_HPP
# define KERNELQUEUECONTROLLER_HPP

#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

class KernelQueueController {
		int kfd;
		struct timespec pollingTime;
		size_t get_events_size;
		size_t set_events_size;
		struct kevent* getEvent;
		struct kevent* setEvent;
	public:
		KernelQueueController(size_t events_size);
		~KernelQueueController();
		size_t	accessEvents();
		void	addEvent(int fd, int16_t event, void* instancePointer);
		void	removeEvent(int fd, int16_t event, void* instancePointer);
		void	enableEvent(int fd, int16_t event, void* instancePointer);
		void	disableEvent(int fd, int16_t event, void* instancePointer);
		void*	getInstanceByEventIndex(int index);
		int		getFdByEventIndex(int index);
		bool	isCloseByEventIndex(int index);
};

#endif
