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

# include				"Utils.hpp"
# include				"Server_Process.hpp"
# include				"Config_Controller.hpp"

extern ConfigController	_cnf(false);
extern ConfigController	_mim(true);

/**
-------------------------------------------------------------
> HTTPMessage

- Protected Variables:
_header_field			map(string, string), store header field
_start_line	  		    string, Save start lines
_msg_body			  	string, Save the message body

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

/**
-------------------------------------------------------------
> RequestMessage : HTTPMessage
*
- Private Variables:
_mth					string, Save method
_rqu					string, Save request url
_dir					string, Save directory
_fil					string, Save file name
_chk					string, Check is this HTTP
_ver					double, HTTP version
*
- Member functions:
Getter:				Method, RequestTarget, UriDirectory. UriFile, HttpVersion
resetMessage		변수 초기화
setHeaderField		_header_field에 페어(Key - Value) 추가
printHeaderField	ㄷ~ㅂ~ㄱ
parseMethod			method 파싱해서 _mth에 저장
                    - GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS, TRACE
parseTarget		    request target 파싱 후 저장
parseIsHTTP		    HTTP/ 체크 및 파싱 후 저장
parseHTTPVersion	HTTP 버전 파싱 후 저장
                    1.0, 1.1, 2.0
parseStartLine		파싱 메인?
parseRequestTarget	Start Line, Header Field 파싱
                    TODO: POST - message body 파싱 필요
printRequestTarget	디~버~깅

------------------------------------------------------------- *
*/

class					RequestMessage : public HTTPMessage
{
	private:
		std::string		_mth;
		std::string		_rqu;
		std::string		_dir;
		std::string		_fil;
		std::string		_chk;
		double			_ver;

	public:
		std::string		getMethod(void)			{ return (_mth); }
		std::string		getRequestTarget(void)	{ return (_rqu); }
		std::string		getUriDir(void)			{ return (_dir); }
		std::string		getUriFile(void)		{ return (_fil); }
		double			getHttpVersion(void)	{ return (_ver); }

		void
			resetMessage(void)
		{
			_start_line = "";
			_header_field.clear();
			_msg_body = "";
			_mth = "";
			_rqu = "";
			_dir = "";
			_fil = "";
			_chk = "";
			_ver = 0.0;
			return ;
		}

        /**
        메소드 파싱.
        GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS, TRACE중에 없으면 에러

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		int
			parseMethod(int _stt, int _end, std::string& _msg)
		{
			*_stt = 0;
			*_end = _msg.find(' ');
			if (*_end == ERROR)
				return (ERROR);

			std::cout	<< _msg.substr(*_stt, *end) << std::endl;
			_mth = _msg.substr(*_stt, *end);
			if (_mth.compare("GET")		== 0 ||
				_mth.compare("POST")	== 0 ||
				_mth.compare("PUT")		== 0 ||
				_mth.compare("PATCH")	== 0 ||
				_mth.compare("DELETE")	== 0 ||
				_mth.compare("HEAD")	== 0 ||
				_mth.compare("CONNECT")	== 0 ||
				_mth.compare("OPTIONS")	== 0 ||
				_mth.compare("TRACE")	== 0)
				return (0);
			return (ERROR);
		}

        /**
        request target 파싱.
        GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS, TRACE중에 없으면 에러

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		int
			parseTarget(int _stt, int _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find(' ', *_stt);
			if (*_end == ERROR)
				return (ERROR);
			
			_rqu = _msg.substr(*_stt, *end);
			if (_rqu.length() > 0 && _rqu.at(0) == '/')
			{
				int _pos_dir = _rqu.find_last_of("/");
				int _pos_fil = _rqu.find_last_of(".");
				if (_pos_dir < _pos_fil)
				{
					_dir = _rqu.substr(0, _pos_dir);
					_fil = _rqu.substr(_pos_dir + 1);
				}
				else
				{
					_dir = _rqu;;
					_fil = _cnf.getContent("index");
				}
				return (0);
			}
			return (ERROR);
		}

        /**
        HTTP/ 파싱

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		int
			parseIsHTTP(int _stt, int _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find(' ', *_stt);
			if (*_end == ERROR)
				return (ERROR);
			
			_chk = _msg.substr(*_stt, *end);
			if (_chk.compare("HTTP/") == 0)
				return (0);
			return (ERROR);
		}

        /**
        HTTP 버전 파싱
        1.0, 1.1, 2.0

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		int
			parseHTTPVersion(int _stt, int _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find('\r', *_stt);
			if (*_end == ERROR)
				return (ERROR);

			_ver = atof(_msg.substr(*_stt, *end).c_str());
			_ver = floor(_ver 10) / 10;
			if (_ver == 1.0 ||
				_ver == 1.1 ||
				_ver == 2.0)
				return (0);
			return (ERROR);
		}

        /**
        스타트라인 파싱 함수 시작파트.
        공용으로 쓰던 _stt, _end 여기서 만듦

        @param  _msg 메시지
        */
		int
			parseStartLine(std::string& _msg)
		{
			int _stt, _end;

			if (parseMethod(&_stt, &_end, _msg) == ERROR)
				return (ERROR);
			if (parseTarget(&_stt, &_end, _msg) == ERROR)
				return (ERROR);
			if (parseIsHTTP(&_stt, &_end, _msg) == ERROR)
				return (ERROR);
			if (parseHTTPVersion(&_stt, &_end, _msg) == ERROR)
				return (ERROR);
			return (_stt + 5);
		}

        /**
        Startline, HeaderField 파싱.

        @param  _msg 메시지
        @todo   parseStartLine이랑 합치기
        */
		int
			parseRequestMessage(int _fdN, std::string& _msg)
		{
			int			_num;

			if ((_num = parseStartLine(_msg)) == ERROR)
			{
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Parsing error" << std::endl;
				return (ERROR);
			}
			parseHeaderField(_msg, _num);
			return		(0);
		}

		void
			printRequestMessage(void)
		{
			std::string _rtn;
			std::cout << "[METHOD] : " << this->_mth << std::endl;
			std::cout << "[DIR]    : " << this->_dir << std::endl;
			std::cout << "[FILE]   : " << this->_fil << std::endl;
			std::cout << "[HTTP]   : " << this->_chk + std::to_string(this->_ver) << std::endl;
			return ;
		}
};

/**
-------------------------------------------------------------
> ResponseMessage : Son of HTTPMessage
*
- why separate with RequestMessage: 'cause they have different start-line
*
- Protected Variables:
_ver					double, HTTP version
_sta					int, status code
_rea					std::string, reason phrase
_ext					std::string, extension
*
- Member functions:
resetMessage			Reset all variables (even parents' variables)
setter(simple)		Setter for simple variables (_ver, _sta, _ext)
setReasonPhrase		Give a reason phrase that fits the status code
setResponseHeader...	Set header field.
*						PROBLEM: set content-length 0 if the file is binary
------------------------------------------------------------- *
*/

class					ResponseMessage : public HTTPMessage
{
	protected:
		double			_ver;
		int				_sta;
		std::string		_rea;
		std::string		_ext;

	public:
		void
			resetMessage(void)
		{
			_start_line = "";
			_header_field.clear();
			_msg_body = "";
			_sta = 0;
			_rea = "";
			_ver = 0.0;
			return ;
		}

		void
			setHTTPVersion(double _dbl)
		{
			_ver = _dbl;
		}

		void
			setStatusCode(int _num)
		{
			_sta = _num;
		}

		void
			setExtension(std::string _str)
		{
			_ext = _str;
		}

		void
			setReasonPhrase(void)
		{
			switch(_sta)
			{
				case 200:	_rea += "OK";		break;
				case 403:	_rea += "Forbidden";	break;
				case 404:	_rea += "Not Found";	break;
				default :	_rea += "Error";		break;
			}
		}

		void
			setResponseHeaderField(void)
		{
			_header_field.insert(std::pair<std::string, std::string>("Content-Type", _mim.getContent(this->_ext)));
			setHeaderField(std::to_string(this->getMessageBody().length()));
			_header_field.insert(std::pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}


};

#endif