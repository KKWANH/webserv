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
_htp					string, Check is this HTTP
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
		std::string		_htp;
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
			_htp = "";
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
			parseMethod(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = 0;
			*_end = _msg.find(' ');
			if (*_end == ERROR)
				return (ERROR);

			std::cout	<< _msg.substr(*_stt, *_end) << std::endl;
			_mth = _msg.substr(*_stt, *_end);
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
			parseTarget(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find(' ', *_stt);
			if (*_end == ERROR)
				return (ERROR);
			
			_rqu = _msg.substr(*_stt, *_end - *_stt);
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
					_dir = _rqu;
					std::cout << "!!" << _dir << std::endl;
					_fil = _config.getContent("index");
					std::cout << "!!" << _dir << std::endl;
				}
				std::cout << "ㅠㅠ" << _dir << std::endl;
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
			parseIsHTTP(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find('/', *_stt);
			if (*_end == ERROR)
				return (ERROR);
			
			_htp = _msg.substr(*_stt, *_end - *_stt + 1);
			if (_htp.compare("HTTP/") == 0)
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
			parseHTTPVersion(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find('\r', *_stt);
			if (*_end == ERROR)
				return (ERROR);

			_ver = atof(_msg.substr(*_stt, *_end).c_str());
			_ver = floor(_ver * 10) / 10;
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
			std::cout << "debug" << std::endl;
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
			parsingRequestMessage(int _fld, std::string& _msg)
		{
			int			_num;

			if ((_num = parseStartLine(_msg)) == ERROR)
			{
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Parsing error" << std::endl;
				close(_fld);
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
			std::cout << "[HTTP]   : " << this->_htp + std::to_string(this->_ver) << std::endl;
			return ;
		}
};

/**
-------------------------------------------------------------
> ResponseMessage : Son of HTTPMessage

- why separate with RequestMessage: 'cause they have different start-line

- Protected Variables:
_ver					double, HTTP version
_sta					int, status code
_rea					std::string, reason phrase
_ext					std::string, extension

- Member functions:
resetMessage			Reset all variables (even parents' variables)
setter(simple)			Setter for simple variables (_ver, _sta, _ext)
setReasonPhrase			Give a reason phrase that fits the status code
setResponseHeader...	Set header field.
						PROBLEM: set content-length 0 if the file is binary
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
			setHttpVersion(double _dbl)
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

		/**
		TODO: parsing(config)로 바꾸기
		*/
		void
			setReasonPhrase(void)
		{
			switch(_sta)
			{
				case 200:	_rea += "OK";			break;
				case 403:	_rea += "Forbidden";	break;
				case 404:	_rea += "Not Found";	break;
				default :	_rea += "Error";		break;
			}
		}

		/**
		Header field의 값을 설정
		TODO: binary file의 경우 content-length가 0으로 설정되는 문제
		*/
		void
			setResponseHeaderField(void)
		{
			_header_field.insert(std::make_pair<std::string, std::string>("Content-Type", _mime.getContent(this->_ext)));
			this->setHeaderField("Content-Length", std::to_string(this->getMessageBody().length()));
			_header_field.insert(std::pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		/**
		StartLine 설정
		*/
		void
			setStartLine(int _status_code, double _http_version)
		{
			this->setHttpVersion(_http_version);
			this->setStatusCode(_status_code);
			this->setReasonPhrase();
			_start_line += "HTTP/";
			_start_line += std::to_string(_http_version).substr(0, 3) + " ";
			_start_line += std::to_string(_status_code) + " ";
			_start_line += this->_rea;
		}

		/**
		@return		Response Message 전체 합쳐서 반환
					(Start-Line, Header-Field, Message-Body)
		*/
		std::string
			makeResponseMessage(void)
		{
			std::map<std::string, std::string>::iterator
				_itr;
			std::string
				_str;
			
			_str	= _start_line
					+ "\r\n";
			_itr	= _header_field.begin();
			while (true)
			{
				_str += _itr->first + ": "
					 +  _itr->second + "\r\n";
				++_itr;
				if (_itr == _header_field.end())
					break ;
			}
			_str += "\r\n";
			_str += this->getMessageBody();
			return (_str);
		}

		/**
		TODO:	static page 폴더 내 index.html 파일 파싱 시, 크기가 큰 파일들 파싱 안 됨
				파일을 전체 받아오는 경우, 파일의 크기만큼 메모리가 할당된다.
				오버플로우의 위험이 있음.
				buffer를 만들고 그 크기만큼 쪼개서 읽고, 전송을 반복하는 방법으로 수정 가능
				binary 파일이 string 타입에 대입 시 제대로 들어가는지 확인 필요
		*/
		void
			setMessageBody(std::string _uri)
		{
			std::cout << "URI: " << _uri << std::endl;
			// if (_ext.compare("jpg") == 0 || _ext.compare("ico") == 0)
			// {
			// 	std::cout << "IMAGE" << std::endl;
			// 	int
			// 		_pos = _uri.find_last_of(".");
			// 	std::string
			// 		_tmp = _uri.substr(_pos - 5);
			// 	std::ifstream _ifs(_uri, std::ifstream::binary);
			// 	if (_ifs)
			// 	{
			// 		_ifs.seekg(0, _ifs.end);
			// 		int
			// 			_len = (int)_ifs.tellg();
			// 		_ifs.seekg(0, _ifs.beg);
			// 		char*
			// 			_buf = new char[_len + 1];
			// 		_ifs.read((char*)_buf, _len);
			// 		std::string _tmp(_buf);
			// 		_msg_body = _tmp;
			// 		_ifs.close();
			// 	}
			// }
			// else
			// {
				std::ifstream
					_fil(_uri);
				std::string
					_lin;
				int 
					_idx = 0;
				while (std::getline(_fil, _lin))
				{
					_msg_body = _msg_body + _lin + "\n";
					++_idx;
					if (_idx == 5)
						std::cout << "BODY: \n" << _msg_body << std::endl;
				}
			// }
		}

		/**
		uri가 dir인지, file인지 확인
		*/
		int
			isDirOrFile(RequestMessage* _rqm, std::string* _uri)
		{
			int
				_flg = 0;
			DIR*
				_dp;
			struct dirent*
				_dir;
			
			*_uri = _config.getContent("root") + _rqm->getUriDir();
			std::cout << "URI: " << _uri->c_str() << std::endl;
			if ((_dp = opendir(_uri->c_str())) == NULL)
			{
				std::cout << "DIR Open Error" << std::endl;
				*_uri = _config.getContent("root") + "/404.html";
				this->setExtension("html");
				return (403);
			}
			if (_rqm->getUriDir().compare("/") != 0)
				*_uri = *_uri + "/";
			*_uri = *_uri + _rqm->getUriFile();
			while ((_dir = readdir(_dp)) != NULL)
			{
				if (_dir->d_ino == 0)
					continue;
				if (strcmp(_rqm->getUriFile().c_str(), _dir->d_name) == 0)
				{
					std::string
						_extension;
					int
						_pos = _rqm->getUriFile().find_last_of(".");
					_extension = _rqm->getUriFile().substr(_pos + 1);
					this->setExtension(_extension);
					_flg = 200;
					break;
				}
			}

			if (_flg != 200)
			{
				*_uri = _config.getContent("root") + "/404.html";
				_flg = 404;
				this->setExtension("html");
			}
			closedir(_dp);
			return (_flg);
		}

		/**
		*/
		static std::string
			setResponseMessage(RequestMessage* _rqm)
		{
			ResponseMessage
				_rpm;
			std::string
				_uri;
			int
				_flg;

			_flg = _rpm.isDirOrFile(_rqm, &_uri);
			_rpm.setStartLine(_flg, _rqm->getHttpVersion());
			_rpm.setMessageBody(_uri);
			_rpm.setResponseHeaderField();
			
			std::string
				_rst = _rpm.makeResponseMessage();

			return (_rst);
		}
};

#endif