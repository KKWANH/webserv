#ifndef KERNELQUEUE_HPP
# define KERNELQUEUE_HPP

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
// #include "ErrorHandler.hpp"
#include "Socket.hpp"

#define KERNELQUEUE_EVENT_SIZE 10000

class KernelQueue {
	int						_kernelQueuefd;
	int						_setEventIndex;
	struct kevent			_getEvent[KERNELQUEUE_EVENT_SIZE];
	struct kevent			_setEvent[KERNELQUEUE_EVENT_SIZE];
	struct timespec			_pollingTime;
	std::map<int, void *>	_pair;

	void	addEvent(int fd, int16_t event, void *tmpPtr)
	{
		EV_SET(&_setEvent[_setEventIndex++], fd, event, EV_ADD | EV_EOF, 0, 0, tmpPtr); // EV_EOF ??
	}

	void	removeEvent(int fd, int16_t event, void *tmpPtr)
	{
		EV_SET(&_setEvent[_setEventIndex++], fd, event, EV_DELETE, 0, 0, tmpPtr);
	}

	public:
	KernelQueue(float pollingTime)
	{
		_kernelQueuefd = kqueue();
		if (_kernelQueuefd == -1)
			throw std::string("kqueue() failed"); // ErrorHander로 바꾸기

		_pollingTime.tv_sec = (long)pollingTime;
		_pollingTime.tv_nsec = long(pollingTime * 1000000000L) % 1000000000L; // polling time 에 대해 알아볼 것
		_setEventIndex = 0;
	}

	~KernelQueue()
	{
		close(_kernelQueuefd);
	}

	int	getEventIndex(void)
	{
		int	result = kevent(_kernelQueuefd, _setEvent, _setEventIndex, _getEvent, KERNELQUEUE_EVENT_SIZE, &_pollingTime);
		if (result == -1)
			throw std::string("Kevent Error") // ErrorHander로 바꾸기
		_setEventIndex = 0;
		return (result);
	}

	void	addReadEvent(int fd, void *tmpPtr)
	{
		addEvent(fd, EVFILT_READ, tmpPtr);
	}

	void	modEventToWriteEvent(int index)
	{
		removeEvent(_getEvent[index].ident, EVFILT_READ, _getEvent[index].udata);
		addEvent(_getEvent[index].ident, EVFILT_WRITE, _getEvent[index].udata);

	}

	void	modEventToReadEvent(int index)
	{
		removeEvent(_getEvent[index].ident, EVFILT_WRITE, _getEvent[index].udata);
		addEvent(_getEvent[index].ident, EVFILT_READ, _getEvent[index].udata);
	}

	bool	isClose(int index) const
	{
		return (_getEvent[index].flags & EV_EOF);
	}

	bool	isReadEvent(int index) const
	{
		return (_getEvent[index].filter == EVFILT_READ);
	}

	bool	isWriteEvent(int index) const
	{
		return (_getEvent[index].filter == EVFILT_WRITE);
	}

	int		getFd(int index) const
	{
		return (_getEvent[index].ident);
	}

	void	*getTmpPtr(int index) const
	{
		return (_getEvent[index].udata);
	}

	void	pairStopMaster(int index) // master?
	{
		if (_pair.find(int(_getEvent[index].ident)) != _pair.end())
			_pair[(int(_getEvent[index].ident))]->stopMaster();
	}

	long	getData(int index)
	{
		return (_getEvent[index].data);
	}

	void	setPairEvent(int masterIndex, int slaveReadFd, bool isRead)
	{
		struct kevent	master;
		struct kevent	slave;

		_pair[_getEvent[masterIndex].ident] = new PairQueue(this->_kernelQueuefd, this->_setEvent, this->_setEventIndex);
		EV_SET(&master, _getEvent[masterIndex].ident, _getEvent[masterIndex].filter, EV_DISABLE, 0, 0, _getEvent[masterIndex].udata);
		EV_SET(&slave, slaveReadFd, isRead ? EVFILT_READ : EVFILT_WRITE, EV_ADD | EV_EOF, 0, 0, reinterpret_cast<void *>(_pair[_getEvent[masterIndex].ident]));
		_pair[_getEvent[masterIndex].ident]->setPairQueeu(master, slave);
		_setEvent[_setEventIndex++] = master;
		_setEvent[_setEventIndex++] = slave;
	}

	void	deletePairEvent(int mstarIndex)
	{
		if (_pair.find(_getEvent[mstarIndex].ident) != _pair.end())
		{
			delete _pair[int(_getEvent[mstarIndex].ident)];
			_pair.erase(int(_getEvent[mstarIndex].ident));
		}
	}

	class PairQueue : public Socket {
		private:
			int				_kernelQueuefd;
			struct kevent	*_master;
			struct kevent	*_slave;
			struct kevent	*_setEvent;
			int				&setEventIndex;
		public:
			PairQueue(int kernelQueuefd, struct kevent *setEvent, int &setEventIndex)
			{
				_kernelQueuefd = kernelQueuefd;
				_master = NULL;
				_slave = NULL;
			}

			virtual ~PairQueue()
			{
				delete _master;
				delete _slave;
			}

			void	setPairQueue(struct kevent master, struct kevent slave)
			{
				_master = new struct kevent(master);
				_slave = new struct kevent(slave);
			}

			void	stopMaster(void)
			{
				_master.flags = EV_DISABLE;
				_slave.flags = EV_ENABLE;
				_setEvent[setEventIndex++] = *_master;
				_setEvent[setEventIndex++] = *_slave;
			}

			void	stopSlave(void)
			{
				_master.flags = EV_ENABLE;
				_slave.flags = EV_DISABLE;
				_setEvent[setEventIndex++] = *_master;
				_setEvent[setEventIndex++] = *_slave;
			}

			virtual int	runSocker(void) // ?
			{
				return (-1);
			}
	};
};

#endif
