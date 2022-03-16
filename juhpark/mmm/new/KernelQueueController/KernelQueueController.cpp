#include "KernelQueueController.hpp"

KernelQueueController::KernelQueueController(size_t events_size) : get_events_size(events_size), set_events_size(0) {
	// polling time = 1 sec
	pollingTime.tv_sec = 1;
	pollingTime.tv_nsec = 0;
	getEvent = new struct kevent[events_size];
	setEvent = new struct kevent[events_size];
	kfd = kqueue();
}
KernelQueueController::~KernelQueueController() {
	delete getEvent;
	delete setEvent;
	close(kfd);
}
size_t KernelQueueController::accessEvents() {
	size_t events = kevent(kfd, setEvent, set_events_size, getEvent, get_events_size, &pollingTime);
	set_events_size = 0;
	return events;
}

void KernelQueueController::addEvent(int fd, int16_t event, void* instancePointer) {
	EV_SET(&setEvent[set_events_size++], fd, event, EV_ADD | EV_EOF, 0, 0, instancePointer);
}

void KernelQueueController::removeEvent(int fd, int16_t event, void* instancePointer) {
    EV_SET(&setEvent[set_events_size++], fd, event, EV_DELETE, 0, 0, instancePointer);
}

void KernelQueueController::enableEvent(int fd, int16_t event, void* instancePointer) {
    EV_SET(&setEvent[set_events_size++], fd, event, EV_ENABLE, 0, 0, instancePointer);
}

void KernelQueueController::disableEvent(int fd, int16_t event, void* instancePointer) {
    EV_SET(&setEvent[set_events_size++], fd, event, EV_DISABLE, 0, 0, instancePointer);
}

void* KernelQueueController::getInstanceByEventIndex(int index) {
	return getEvent[index].udata;
}

int KernelQueueController::getFdByEventIndex(int index) {
	return getEvent[index].ident;
}

bool KernelQueueController::isCloseByEventIndex(int index) {
   	return (getEvent[index].flags & EV_EOF);
}