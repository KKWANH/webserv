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
# include				"../http_message/RequestMsg_Controller.hpp"
# include				"../http_message/ResponseMsg_Controller.hpp"
# include				"../server_process/Server_Process.hpp"
# include				"../error/Error_Handler.hpp"

class					KQueueController
{
	private:
		struct timespec*				_timeout;						// timeout 시간을 지정하기 위한 구조체
		int								_kq;							// kqueue descriptor
		struct kevent					_event_list[BUFSIZ];			// 작업을 돌려받는 list
		struct kevent					_change_list[BUFSIZ];			// 작업을 추가하기 위한 list
		int								_change_cnt;					// 추가할 작업 수
		std::map<int, RequestMessage>	_request_msg;					// 통신 시 주고받는 Request message 데이터를 임시로 저장할 장소
		int								_polling_cnt;					// event 수
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
				throw ErrorHandler(__FILE__, __func__, __LINE__, "fcntl error");
			
			return (0);
		}

		void		clearChangeList() {
			for (int _idx = 0; _idx < _change_cnt; _idx++)
				EV_SET(&_change_list[_idx], 0, 0, 0, 0, 0, NULL);
			return ;
		}

		/**
		파라미터로 주어지는 fd에 대한 EV_SET
		READ ADD | ENABLE and WRITE ADD | DISABLE
		*/
		void
			setReadKqueue(int _fld)
		{
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_READ,  EV_ADD | EV_ENABLE,  0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
		}

		/**
		파라미터로 주어지는 fd에 대한 EV_SET
		READ ADD | ENABLE and WRITE ADD | DISABLE
		*/
		void
			setWriteKqueue(int _fld)
		{
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_READ,  EV_ADD | EV_DISABLE, 0, 0, NULL);
			this->increaseChangeCount();
			EV_SET(this->getChangeList() + this->getChangeCount(), _fld, EVFILT_WRITE, EV_ADD | EV_ENABLE,  0, 0, NULL);
			this->increaseChangeCount();
		}

		/**
		requestMessage 내 fd-RequestMessage 쌍 제거
		*/
		void
			removeRequestMessage(int _fld)
		{
			_request_msg[_fld].resetMessage();
			_request_msg.erase(_fld);
			_response_msg.erase(_fld);
			_response_msg_siz.erase(_fld);
		}

		/**
		requestMessage 내 fd-RequestMessage 쌍 형태로 삽입
		*/
		void
			addRequestMessage(int _fld)
		{
			RequestMessage
				_tmp_msg;
			std::cout << "_-----------" << "[" << _fld << "]" << "-----------_" << std::endl;
			std::cout << _tmp_buf[_fld] << std::endl;
			std::cout << "_------------------------------_" << std::endl;
			_tmp_msg.parsingRequestMessage(_fld, this->_tmp_buf[_fld]);
			_request_msg.insert(std::make_pair(_fld, _tmp_msg));
			this->_tmp_buf[_fld] = "";
			return ;
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