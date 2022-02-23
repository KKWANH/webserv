#ifndef					HTTPMSG_CONTROLLER_HPP
# define				HTTPMSG_CONTROLLER_HPP

# include				<string>
# include				<map>
# include				<iostream>
# include				<sys/socket.h>
# include				<arpa/inet.h>
# include				<unistd.h>
# include				<utility>
# include				<sstream>
# include				<fstream>
# include				<dirent.h>
# include				<cmath>
# include				<sys/stat.h>

# include				"../utils/Utils.hpp"
# include				"../server_process/Server_Process.hpp"
# include				"../parse/Config_Controller.hpp"

extern ConfigController	_config;
extern ConfigController	_mime;

/**
-------------------------------------------------------------
> HTTPMessage

- Protected Variables:
_header_field			Request/Response 동일
_msg_body			  	Request/Response 동일
_start_line	  		    Request/Response에 따라 다르게 구성되어있음.
						두 가지 상태에 따라 HTTPMessage 클래스를 상속하여 사용

- Member functions:
Getter:				    MessageBody, StartLine
parseHeaderField		Parse header field and save to _header_field(map).
setHeaderField		    Insert new pair to _header_field
printHeaderField		We know it remains for debugging
-------------------------------------------------------------
*/

class					HTTPMessage
{
	protected:
		std::map<std::string, std::string>
			_header_field;
		std::string
			_start_line;
		std::string
			_msg_body;
	
	public:
		std::string		getMessageBody(void)	{ return (_msg_body); }
		std::string		getStartLine(void)		{ return (_start_line); }

        /**
        Parse header field and save to _header_field(map).

        @return         last_index + 1
        @param  _msg    클라이언트 측에서 전송하는 데이터
        @param  pos     start_line을 parsing하고 난 이후의 위
        @todo           
        */
		int
			parseHeaderField(std::string _msg, int &_pos)
		{
			int			_stt = _pos, _lst;
			std::string	_key, _val;

			// if header_field doesn't exist
			if ((int)_msg.length() == _stt)
				return (0);
			
			while (true)
			{
				_lst = _msg.find(':', _stt);
				_key = _msg.substr(_stt, _lst - _stt);
				_stt = _lst + 1;
				if (_msg.at(_lst + 1) == ' ')
					++_stt;
				_lst = _msg.find("\r\n", _stt);
				_val = _msg.substr(_stt, _lst - _stt);
				_header_field.insert(std::pair<std::string, std::string>(_key, _val));
				_stt = _lst + 2;
				// CRLF
				if (_msg.at(_stt) == '\r' && _msg.at(_stt + 1) == '\n')
					break ;
			}

			return (_stt + 2);
		}

		void
			setHeaderField(std::string _key, std::string _val)
		{
			_header_field.insert(std::pair<std::string, std::string>(_key, _val));
		}

		void
			printHeaderField(void)
		{
			std::map<std::string, std::string>::iterator _itr;
			_itr = _header_field.begin();
			std::cout		<< ANSI_BLU << "[INF] "
							<< ANSI_RES << "debugging" << std::endl;
			while (_itr != _header_field.end())
			{
				std::cout << _itr->first << " : " << _itr->second << std::endl;
				++_itr;
			}
		}
};

#endif