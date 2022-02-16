#ifndef					KQUEUE_CONTROLLER_HPP
# define				KQUEUE_CONTROLLER_HPP

# include				<iostream>
# include				<sys/socket.h>
# include				<sys/event.h>
# include				<fcntl.h>
# include				<vector>
# include				<map>

# include				"../utils/Utils.hpp"
# include				"../http_message/HTTPMsg_Controller.hpp"
# include				"../server_process/Server_Process.hpp"

class					KQueueController
{
	private:
		struct timespec*				_timeout;
		int								_kq;
		struct kevent					_event_list[BUFSIZ];
		struct kevent					_change_list[BUFSIZ];
		int								_change_cnt;
		std::map<int, RequestMessage>	_request_msg;
		int								_polling_cnt;
		std::string						_tmp_buf[BUFSIZ];
		std::map<int, std::string>		_response_msg;
		std::map<int, int>				_response_msg_siz;
	
	public:
		struct timespec*				getTimeout()					{ return (_timeout); }
		int								getKq()							{ return (_kq); }
		struct kevent*					getEventList()					{ return (_event_list); }
		struct kevent*					getEventList(int _num)			{ return (&_event_list[_num]); }
		struct kevent*					getChangeList()					{ return (_change_list); }
		struct kevent*					getChangeList(int _num)			{ return (&_change_list[_num]); }
		int								getChangeCount()				{ return (_change_cnt); }
		int								getPollingCount()				{ return (_polling_cnt); }
		RequestMessage*					getRequestMessage(int _fd) 		{ return (&(_request_msg.find(_fd)->second)); }

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
			_request_msg[_fld].resetMessage();
			_request_msg.erase(_fld);
			_response_msg.erase(_fld);
			_response_msg_siz.erase(_fld);
		}

		int
			addRequestMessage(int _fld)
		{
			RequestMessage
				_tmp_msg;
			std::cout << "_-----------" << "[" << _fld << "]" << "-----------_" << std::endl;
			std::cout << _tmp_buf[_fld] << std::endl;
			std::cout << "_------------------------------_" << std::endl;
			if (_tmp_msg.parsingRequestMessage(_fld, this->_tmp_buf[_fld]) == ERROR)
				return (ERROR);
			_request_msg.insert(std::make_pair(_fld, _tmp_msg));
			this->_tmp_buf[_fld] = "";
			return (0);
		}

		void
			sumMessage(int _fld, char* _buf)
		{
			std::string
				_str_msg(_buf);
			this->_tmp_buf[_fld] += _str_msg;
		}

		void
			saveResponseMessage(int _fld, std::string& _msg)
		{
			this->_response_msg.insert(		std::make_pair(_fld, _msg));
			this->_response_msg_siz.insert(	std::make_pair(_fld, _msg.size()));
		}

		int
			writeResponseMessage(int _fld, int _buf_siz)
		{
			int
				_wrt_siz =
					((int)_response_msg[_fld].size() < _buf_siz) ?
						(int)_response_msg[_fld].size() :
						_buf_siz;
			int
				_cnt_wrt =
					write(_fld, _response_msg[_fld].c_str(), _wrt_siz);
			if (_wrt_siz != _buf_siz)
				return (_cnt_wrt);
			_response_msg[_fld] = _response_msg[_fld].substr(_buf_siz);
			_response_msg_siz[_fld] -= _buf_siz;
			return (_cnt_wrt);
		}
};


#endif