#ifndef					REQUESTMSG_CONTROLLER
# define				REQUESTMSG_CONTROLLER

# include				"./HTTPMsg_Controller.hpp"

/**
-------------------------------------------------------------
> RequestMessage : HTTPMessage

- Member functions:
Getter:				Method, RequestTarget, UriDirectory. UriFile, HttpVersion
resetMessage		변수 초기화
setHeaderField		_header_field에 페어(Key - Value) 추가
printHeaderField	ㄷ~ㅂ~ㄱ
parseMethod			method 파싱해서 _method에 저장
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
		std::string		_method;
		std::string		_request_target;
		std::string		_uri_directory;
		std::string		_uri_file;
		std::string		_chk_HTTP;
		std::string		_query;
		double			_version;
		bool			_is_CGI;

	public:
		std::string		getMethod(void)			{ return (_method); }
		std::string		getRequestTarget(void)	{ return (_request_target); }
		std::string		getUriDir(void)			{ return (_uri_directory); }
		std::string		getUriFile(void)		{ return (_uri_file); }
		double			getHttpVersion(void)	{ return (_version); }
		std::string		getQueryString(void)	{ return (_query); }
		bool			getIsCGI(void)			{ return (_is_CGI); }
		void			setIsCGI(bool _flg)		{ this->_is_CGI = _flg; }

		void
			resetMessage(void)
		{
			_header_field.clear();
			_start_line		= "";
			_msg_body		= "";
			_method			= "";
			_request_target = "";
			_uri_directory	= "";
			_uri_file		= "";
			_chk_HTTP		= "";
			_version		= 0.0;
		}

        /**
        메소드 파싱.
        GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS, TRACE중에 없으면 에러

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		void
			parseMethod(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = 0;
			*_end = _msg.find(' ');
			if (*_end == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request Message");
			std::cout	<< _msg.substr(*_stt, *_end) << std::endl;
			_method = _msg.substr(*_stt, *_end);
			if (_method.compare("GET")		== 0 ||
				_method.compare("POST")		== 0 ||
				_method.compare("PUT")		== 0 ||
				_method.compare("PATCH")	== 0 ||
				_method.compare("DELETE")	== 0 ||
				_method.compare("HEAD")		== 0 ||
				_method.compare("CONNECT")	== 0 ||
				_method.compare("OPTIONS")	== 0 ||
				_method.compare("TRACE")	== 0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "HTTP Method parsing error in request message");
		}

        /**
        request target 파싱.
        GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS, TRACE중에 없으면 에러

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		void
			parseTarget(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find(' ', *_stt);
			if (*_end == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no URI in Request Message");
			
			_request_target = _msg.substr(*_stt, *_end - *_stt);
			if (_request_target.length() > 0 && _request_target.at(0) == '/')
			{
				int		_pos_uri_directory = _request_target.find_last_of("/");
				int		_pos_uri_file = _request_target.find_last_of(".");
				size_t	_pos_query = _request_target.find("?");
				if (_pos_uri_directory < _pos_uri_file)
				{
					this->_uri_directory = _request_target.substr(0, _pos_uri_directory);
					if (_pos_query == std::string::npos)
					{
						this->_is_CGI = false;
						this->_uri_file = _request_target.substr(_pos_uri_directory + 1);
					}
					else
					{
						this->_is_CGI = true;
						this->_uri_file = _request_target.substr(_pos_uri_directory + 1, _pos_query - _pos_uri_directory - 1);
						this->_query = _request_target.substr(_pos_query + 1);
					}
				}
				else
				{
					this->_is_CGI = false;
					this->_uri_directory = _request_target;
					this->_uri_file = _config.getContent("index");
				}
				return ;
			}
			throw ErrorHandler(__FILE__, __func__, __LINE__, "URI parsing error in request message");
		}

        /**
        HTTP/ 파싱

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		void
			parseIsHTTP(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find('/', *_stt);
			if (*_end == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol information in Request Message");
			
			_chk_HTTP = _msg.substr(*_stt, *_end - *_stt + 1);
			if (_chk_HTTP.compare("HTTP/") == 0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
		}

        /**
        HTTP 버전 파싱
        1.0, 1.1, 2.0

        @param  _stt |
        @param  _end | 공용 변수
        @param  _msg |
        */
		void
			parseHTTPVersion(int* _stt, int* _end, std::string& _msg)
		{
			*_stt = *_end + 1;
			*_end = _msg.find('\r', *_stt);
			if (*_end == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol version in Request Message");

			_version = atof(_msg.substr(*_stt, *_end).c_str());
			_version = floor(_version * 10) / 10;
			if (_version == 1.0 ||
				_version == 1.1 ||
				_version == 2.0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
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
			this->parseMethod(&_stt, &_end, _msg);
			this->parseTarget(&_stt, &_end, _msg);
			this->parseIsHTTP(&_stt, &_end, _msg);
			this->parseHTTPVersion(&_stt, &_end, _msg);
			return (_stt + 5);
		}

        /**
        Startline, HeaderField 파싱.

        @param  _msg 메시지
        @todo   parseStartLine이랑 합치기
		@todo	POST의 경우 message body 파싱 필요함
        */
		void
			parsingRequestMessage(int _fld, std::string& _msg)
		{
			int			_num;

			if ((_num = parseStartLine(_msg)) == ERROR)
			{
				close(_fld);
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Start line parsing error in Request message");
			}
			this->parseHeaderField(_msg, _num);
			return ;
		}

};

#endif