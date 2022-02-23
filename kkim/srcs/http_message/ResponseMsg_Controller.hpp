#ifndef					RESPONSEMSG_CONTROLLER
# define				RESPONSEMSG_CONTROLLER

# include				"./HTTPMsg_Controller.hpp"
# include				"./RequestMsg_Controller.hpp"
# include				"./../cgi/CGI_Process.hpp"

/**
-------------------------------------------------------------
> ResponseMessage : Son of HTTPMessage

- why separate with RequestMessage: 'cause they have different start-line

- Member functions:
resetMessage			Reset all variables (even parents' variables)
setter(simple)			Setter for simple variables (_version, _status_code, _extension)
setReasonPhrase			Give a reason phrase that fits the status code
setResponseHeader...	Set header field.
						PROBLEM: set content-length 0 if the file is binary
------------------------------------------------------------- *
*/

class					ResponseMessage : public HTTPMessage
{
	protected:
		double			_version;
		int				_status_code;
		std::string		_why;
		std::string		_extension;
		bool			_chk_binary;
		int				_siz_binary;

	public:
		void
			resetMessage(void)
		{
			_start_line = "";
			_header_field.clear();
			_msg_body = "";
			_status_code = 0;
			_why = "";
			_version = 0.0;
			return ;
		}

		void
			setHttpVersion(double _dbl)
		{
			_version = _dbl;
		}

		void
			setStatusCode(int _num)
		{
			_status_code = _num;
		}

		void
			setExtension(std::string _str)
		{
			_extension = _str;
		}

		/**
		TODO: parsing(config)로 바꾸기
		*/
		void
			setReasonPhrase(void)
		{
			switch(_status_code)
			{
				case 200:	_why += "OK";			break;
				case 403:	_why += "Forbidden";	break;
				case 404:	_why += "Not Found";	break;
				default :	_why += "Error";		break;
			}
		}

		/**
		Header field의 값을 설정
		TODO: binary file의 경우 content-length가 0으로 설정되는 문제
		*/
		void
			setResponseHeaderField(void)
		{
			_header_field.insert(std::make_pair<std::string, std::string>("Content-Type", _mime.getContent(this->_extension)));
			this->setHeaderField("Content-Length", std::to_string(this->getMessageBody().length()));
			_header_field.insert(std::pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		/**
		StartLine 설정
		*/
		void
			setStartLine(int _status_codetus_code, double _http_versionsion)
		{
			this->setHttpVersion(_http_versionsion);
			this->setStatusCode(_status_codetus_code);
			this->setReasonPhrase();
			_start_line += "HTTP/";
			_start_line += std::to_string(_http_versionsion).substr(0, 3) + " ";
			_start_line += std::to_string(_status_codetus_code) + " ";
			_start_line += this->_why;
		}

		/**
		@return		Response Message 전체 합쳐서 반환
					(Start-Line, Header-Field, Message-Body)
		*/
		std::string
			makeResponseMessage(bool is_cgi)
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
			if (is_cgi == false)
			{
				_str += "\r\n";
				_str += _msg_body;
			}
			else
				_str += _msg_body;
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
			setMessageBody(RequestMessage* _request_msg, std::string _uri)
		{
			if (_request_msg->getIsCGI())
			{
				CGIProcess
					_cgi;
				_cgi.setEnvp(_request_msg);
				_cgi.CGIprocess(0);
				std::cout << "cgi.getOutputPair() -> " << _cgi.getOutputPair() << std::endl;
				
				char
					_buffer[1024];
				int
					_num;
				while ((_num = read(_cgi.getOutputPair(), _buffer, 1024)) > 0)
				{
					_buffer[_num] = '\0';
					_msg_body += std::string(_buffer);
					std::cout << "buf -> " << _msg_body << std::endl;
				}
				if (_num < 0)
					throw ErrorHandler(__FILE__, __func__, __LINE__, "error in read cgi");
				std::cout << _msg_body << std::endl;
			}
			else
			{
				std::ifstream
					_file(_uri);
				std::string
					_line;
				
				if (_file.is_open())
				{
					_file.seekg(0, std::ios::end);
					int
						_size = _file.tellg();
					_line.resize(_size);
					_file.seekg(0, std::ios::beg);
					_file.read(&_line[0], _size);
					_msg_body = _line;
				}
				while (std::getline(_file, _line))
					_msg_body = _msg_body + _line + "\n";
			}
			return ;
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
			
			if (_rqm->getIsCGI() == false)
				*_uri = _config.getContent("root") + _rqm->getUriDir();
			else
				*_uri = "." + _rqm->getUriDir();
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
						_extensionension;
					int
						_pos = _rqm->getUriFile().find_last_of(".");
					_extensionension = _rqm->getUriFile().substr(_pos + 1);
					this->setExtension(_extensionension);
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
			_rpm.setMessageBody(_rqm, _uri);
			_rpm.setResponseHeaderField();
			
			std::string
				_rst = _rpm.makeResponseMessage(_rqm->getIsCGI());

			return (_rst);
		}
};

#endif