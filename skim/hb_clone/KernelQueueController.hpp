#ifndef KERNERQUEUECONTROLLER_HPP
# define KERNERQUEUECONTROLLER_HPP

#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <map>
#include "requestMsgController.hpp"

#define BUFSIZE 1024

class KernelQueueController {
	private:
		struct timespec					*timeout;
		int								kq;
		struct kevent					event_list[BUFSIZE];
		struct kevent					change_list[BUFSIZE];
		int								change_count;
		std::map<int, requestMsg>		requestMessage;
		int								polling_count;

	public:
		struct timespec	*getTimeout(void) { return (timeout); }
		int				getKq(void) { return (kq); }
		struct kevent	*getEventList(void) { return (event_list); }
		struct kevent	*getEventList(int num) { return (&event_list[num]); }
		struct kevent	*getChangeList(void) { return (change_list); }
		struct kevent	*getChangeList(int num) { return (&change_list[num]); }
		int				getChangeCount(void) { return (change_count); }
		int				getPollingCount(void) { return (polling_count); }
		requestMsg		*getRequestMessage(int fd) { return (&requestMessage[fd]); }

		void			increaseChangeCount(void) { change_count++; }
		void			increaseChangeCount(int num) { change_count += num; }
		void			decreaseChangeCount(void) { change_count--; }
		void			resetChangeCount(void) { change_count = 0; }
		void			increasePollingCount(void) { polling_count++; }
		void			decreasePollingCount(void) { polling_count--; }
		void			setPollingCount(int num) { polling_count = num; }

		int				init(int serv_sock)
		{
			kq = kqueue();
			struct timespec	tmout = {5, 0};
			timeout = &tmout;

			EV_SET(&change_list[0], serv_sock, EVFILT_READ, EV_ADD  | EV_ENABLE, 0, 0, NULL);
			change_count = 1;

			if (fcntl(serv_sock, F_SETFL, O_NONBLOCK) == -1)
				return (-1);
			return (0);
		}

		void			clearChangeList(void)
		{
			for (int i = 0; i < change_count; i++)
				EV_SET(&change_list[i], 0, 0, 0, 0, 0, NULL);
		}

		void			setReadKqueue(int fd)
		{
			EV_SET(getChangeList() + getChangeCount(), fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(getChangeList() + getChangeCount(), fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			return ;
		}

		void			setWriteKqueue(int fd)
		{
			EV_SET(getChangeList() + getChangeCount(), fd, EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(getChangeList() + getChangeCount(), fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			this->increaseChangeCount();
			return ;
		}

		void			removeRequestMessage(int fd)
		{
			requestMessage[fd].resetMessage();
			requestMessage.erase(fd);
		}

		int				addRequestMsg(int fd, char *buf)
		{
			std::string		stringMessage(buf);
			requestMsg		tempMessage;

			if (tempMessage.parsingRequestMsg(fd, stringMessage) == -1)
				return (-1);
			requestMessage.insert(std::make_pair(fd, tempMessage));
			return (0);
		}
};

#endif
