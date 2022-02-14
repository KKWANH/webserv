#ifndef KERNELQUEUECONTROLLER_HPP
# define KERNELQUEUECONTROLLER_HPP

#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <map>
#include "./../HTTPMessageController/HTTPMessageController.hpp"
#include "./../ServerProcessController/ServerProcess.hpp"

class KernelQueueController {
	private:
		struct timespec*								timeout;								// timeout 시간을 지정하기 위한 구조체
		int												kq;										// kqueue descriptor
		struct kevent									event_list[BUFSIZ];						// 작업을 돌려받는 list
		struct kevent									change_list[BUFSIZ];					// 작업을 추가하기 위한 list
		int												change_count;							// 추가할 작업 수
		std::map<int, RequestMessage>					requestMessage;							// 통신 시 주고받는 Request message 데이터를 임시로 저장할 장소
		int												polling_count;							// event 수
		std::string										tempBuf[BUFSIZ];

	public:
		struct timespec*								getTimeout()						{ return (timeout); }
		int												getKq()								{ return (kq); }
		struct kevent*									getEventList()						{ return (event_list); }
		struct kevent*									getEventList(int num)				{ return (&event_list[num]); }
		struct kevent*									getChangeList()						{ return (change_list); }
		struct kevent*									getChangeList(int num)				{ return (&change_list[num]); }
		int												getChangeCount()					{ return (change_count); }
		int												getPollingCount()					{ return (polling_count); }
		RequestMessage*									getRequestMessage(int fd) 			{ return (&(requestMessage.find(fd)->second)); }

		void											increaseChangeCount()				{ change_count++; }
		void											increaseChangeCount(int num)		{ change_count += num; }
		void											decreaseChangeCount()				{ change_count--; }
		void											resetChangeCount()					{ change_count = 0; }

		void											increasePollingCount()				{ polling_count++; }
		void											decreasePollingCount()				{ polling_count--; }
		void											setPollingCount(int num)			{ polling_count = num; }

		int			init(int s_socket) {
			// init kqueue
			kq = kqueue();
			struct timespec tmout = {5, 0};
			timeout = &tmout;
			// settings for the server
			EV_SET(&change_list[0], s_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			change_count = 1;

			// set non-blocking
			if (fcntl(s_socket, F_SETFL, O_NONBLOCK) == -1)
				return (-1);
			return (0);
		}

		void		clearChangeList() {
			for (int i = 0; i < change_count; i++)
				EV_SET(&change_list[i], 0, 0, 0, 0, 0, NULL);
			return ;
		}

		// 파라미터로 주어지는 fd에 대한 EV_SET
		// READ ADD | ENABLE and WRITE ADD | DISABLE
		void				setReadKqueue(int fd) {
			EV_SET(this->getChangeList() + this->getChangeCount(), fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			return ;
		}

		// 파라미터로 주어지는 fd에 대한 EV_SET
		// READ DISABLE and WRITE ENABLE
		void				setWriteKqueue(int fd) {
			EV_SET(this->getChangeList() + this->getChangeCount(), fd, EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			this->increaseChangeCount();
			return ;
		}

		// requestMessage 내 fd-RequestMessage 쌍 제거
		void				removeRequestMessage(int fd) {
			requestMessage[fd].resetMessage();
			requestMessage.erase(fd);
		}

		// requestMessage 내 fd-RequestMessage 쌍 형태로 삽입
		int						addRequestMessage(int fd) {
			RequestMessage tempMessage;
			std::cout << "_----------[" << fd << "]MSG-------" << std::endl;
			std::cout << tempBuf[fd] << std::endl;
			std::cout << "_---------------------" << std::endl;
			if (tempMessage.parsingRequestMessage(fd, this->tempBuf[fd]) == -1)
				return (-1);
			requestMessage.insert(std::make_pair(fd, tempMessage));
			this->tempBuf[fd] = "";
			return (0);
		}

		void					sumMessage(int fd, char* buf) {
			std::string	stringMessage(buf);
			this->tempBuf[fd] += stringMessage;
			return ;
		}
};

#endif
