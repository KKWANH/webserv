#ifndef					KQUEUE_CONTROLLER_HPP
# define				KQUEUE_CONTROLLER_HPP

# include				<iostream>
# include				<sys/socket.h>
# include				<sys/event.h>
# include				<fcntl.h>
# include				<vector>
# include				<map>

# include				"Utils.hpp"
# include				"HTTPMsg_Controller.hpp"
# include				"Server_Process.hpp"

class					KQueueController
{
	private:
		struct timespec*				_timeout;
		int								_kq;
		struct kevent					_event_list[BUFSIZ];
		struct kevent					_change_list[BUFSIZ];
		int								_change_cnt;
		std::map<int, RequestMessage>	_requestMessage;
		int								_polling_cnt;
	
	public:
		struct timespec*				getTimeout()					{ return (_timeout); }
		int								getKq()							{ return (_kq); }
		struct kevent*					getEventList()					{ return (_event_list); }
		struct kevent*					getEventList(int _num)			{ return (&_event_list[_num]); }
		struct kevent*					getChangeList()					{ return (_change_list); }
		struct kevent*					getChangeList(int _num)			{ return (&_change_list[_num]); }
		int								getChangeCount()				{ return (_change_cnt); }
		int								getPollingCount()				{ return (_polling_cnt); }
		RequestMessage*					getRequestMessage(int _fd) 		{ return (&(_requestMessage.find(_fd)->second)); }

		void							increaseChangeCount()			{ _change_cnt++; }
		void							increaseChangeCount(int num)	{ _change_cnt += num; }
		void							decreaseChangeCount()			{ _change_cnt--; }
		void							resetChangeCount()				{ _change_cnt = 0; }
		void							increasePollingCount()			{ _polling_cnt++; }
		void							decreasePollingCount()			{ _polling_cnt--; }
		void							setPollingCount(int num)		{ _polling_cnt = num; }

		int
			init(int _sck)
		{
			_kq = kqueue();
			struct timespec _tmout = {5, 0};
			_timeout = &_tmout;
			// settings for the server
			EV_SET(&_change_list[0], _sck, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			_change_cnt = 1;	

			// set non-blocking
			if (fcntl(_sck, F_SETFL, O_NONBLOCK) == -1)
				return (-1);
			return (0);
		}

		void		clearChangeList() {
			for (int _idx = 0; _idx < _change_cnt; _idx++)
				EV_SET(&_change_list[_idx], 0, 0, 0, 0, 0, NULL);
			return ;
		}

		/**
		*/
		void
			setReadKqueue(int _fld)
		{
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
		}

		void
			setWriteKqueue(int _fld)
		{
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_READ,  EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_WRITE, EV_ADD | EV_ENABLE,  0, 0, NULL);
			this->increaseChangeCount();
		}

		void
			removeRequestMessage(int _fld)
		{
			_requestMessage[_fld].resetMessage();
			_requestMessage.erase(_fld);
		}

		int
			addRequestMessage(int _fld, char* _buf) {
			std::string
				_strmsg(_buf);
			RequestMessage
				_tmpmsg;
			if (_tmpmsg.parsingRequestMessage(_fld, _strmsg) == -1)
				return (-1);
			_requestMessage.insert(std::make_pair(_fld, _tmpmsg));
			return (0);
		}
};

#endif