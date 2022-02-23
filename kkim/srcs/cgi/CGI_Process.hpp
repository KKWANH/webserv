#ifndef					CGI_PROCESS_HPP
# define				CGI_PROCESS_HPP

# include				<iostream>
# include				<map>
# include				<unistd.h>
# include				<cstdlib>
# include				<cstring>
# include				<signal.h>

# include				"../error/Error_Handler.hpp"
# include				"../http_message/HTTPMsg_Controller.hpp"

class					CGIProcess
{
	private:
		char**
			_env;
		char**
			_arg;
		pid_t
			_pid;
		int
			_pair_input[2];
		int
			_pair_output[2];
		
		char**
			generateEnvp(std::map<std::string, std::string> _map)
		{
			char**
				_rst = new char*[_map.size() + 1];
			int
				_idx = 0;
			std::map<std::string, std::string>::iterator
				_itr;
			
			for (_itr = _map.begin(); _itr != _map.end(); ++_itr)
			{
				_rst[_idx] = new char[(_itr->first.length() + _itr->second.length() + 2)];
				strcpy(_rst[_idx], _itr->first.c_str());
				strcat(_rst[_idx], "=");
				strcat(_rst[_idx], _itr->second.c_str());
				_idx++;
			}
			_rst[_idx] = NULL;
			return _rst;
		}

		void
			setCGIarg(void)
		{
			this->_arg				= new char*[4];
			this->_arg[0]			= new char[22];
			strcpy(this->_arg[0],	"./../test/php-cgi");
			this->_arg[1]			= new char[25];
			strcpy(this->_arg[1],	"./../test/sample.php");
			this->_arg[2]			= new char[10];
			strcpy(this->_arg[2],	"var=1234");
			this->_arg[3]			= NULL;
		}

	public:
		void
			setEnvp(RequestMessage* _request_msg)
		{
			std::map<std::string, std::string> _env_map;

			// local 환경변수에 이미 존재하는 아이들
            _env_map[std::string("USER")] = std::string(std::getenv("USER"));
            _env_map[std::string("PATH")] = std::string(std::getenv("PATH"));
            _env_map[std::string("LANG")] = std::string(std::getenv("LANG"));
            _env_map[std::string("PWD")] = std::string(std::getenv("PWD"));

            // parsing으로 가져온 아이들
            _env_map[std::string("REQUEST_METHOD")] = _request_msg->getMethod();
            // _env_map[std::string("PATH_INFO")] = _request_msg->getUriDir(); // /data/index.html
            _env_map[std::string("PATH_INFO")] = std::string("/index.html");
            _env_map[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
            _env_map[std::string("REQUEST_SCHEME")] = _request_msg->getMethod();
            _env_map[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
            _env_map[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

            // _env_map[std::string("CONTENT_TYPE")] = _request_msg->_reqContentType;
            _env_map[std::string("CONTENT_TYPE")] = "application/x-www-form-urlencoded";
            // _env_map[std::string("CONTENT_LENGTH")] = _request_msg->_reqContentLength;
            _env_map[std::string("CONTENT_LENGTH")] = std::to_string(22);

            // _env_map[std::string("REMOTE_ADDR")] = _request_msg->_clientIP; // server socket addr
            // _env_map[std::string("REMOTE_PORT")] = _request_msg->_clientPort; // client port
            // _env_map[std::string("SERVER_ADDR")] = _request_msg->_hostIP; // client socket addr
            // _env_map[std::string("SERVER_PORT")] = _request_msg->_hostPort; // host port

            _env_map[std::string("QUERY_STRING")] = _request_msg->getQueryString();

            _env_map[std::string("SCRIPT_NAME")] = _request_msg->getUriFile();

            // _env_map[std::string("REQUEST_URI")] = _request_msg->_originURI;
            _env_map[std::string("DOCUMENT_ROOT")] = "/Users/kimkwanho/Documents/Programming/42Seoul/42cursus/05_webserv/webserv/kkim/cgiBinary";
            _env_map[std::string("REQUEST_URI")] = "/Users/kimkwanho/Documents/Programming/42Seoul/42cursus/05_webserv/webserv/kkim/cgiBinary/sample.php";
            _env_map[std::string("DOCUMENT_URI")] = "/Users/kimkwanho/Documents/Programming/42Seoul/42cursus/05_webserv/webserv/kkim/cgiBinary/sample.php";
            // _env_map[std::string("SERVER_NAME")] = _request_msg->_serverName; // config 파일의 서버 이름
            _env_map[std::string("SCRIPT_FILENAME")] = "/Users/kimkwanho/Documents/Programming/42Seoul/42cursus/05_webserv/webserv/kkim/cgiBinary/sample.php";

            // for (std::map<std::string, std::string>::iterator iter = _request_msg->_HTTPCGIENV.begin(); iter != _request_msg->_HTTPCGIENV.end(); iter++) {
            //     _env_map[iter->first] = iter->second;
            // } // CGI binary 파일의 헤더 (HTTP_)
            // request의 헤더가 cgi 의 환경변수로 들어가야 한다.
            
            _env = generateEnvp(_env_map);
            setCGIarg();
		}

		~CGIProcess(void)
		{
			int
				_status;
			
			if (_pid > 0)
			{
				_status = waitpid(_pid, &_status, 0);
				if (_status & 0177)
					kill(_pid, SIGTERM);
			}
			if (_env != NULL)
			{
				int
					_idx = 0;
				while (_env[_idx] != NULL)
				{
					delete _env[_idx];
					++_idx;
				}
				delete _env;
			}
		}

		int&
			getInputPair(void)
		{
			return _pair_input[1];
		}

		int&
			getOutputPair(void)
		{
			return _pair_output[0];
		}

		void
			CGIprocess(int _fld)
		{
			int _tgt;

			if (pipe(this->_pair_input) == ERROR ||
				pipe(this->_pair_output) == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Pipe making error");
			_tgt = (_fld != 0) ? _fld : this->_pair_input[0];
			if (_pid == 0)
			{
				if ((dup2(_tgt, STDIN_FILENO) == ERROR) ||
					(dup2(this->_pair_output[1], STDOUT_FILENO) == ERROR))
					throw ErrorHandler(__FILE__, __func__, __LINE__, "duplicate file descriptor error");
				if ((close(this->_pair_input[1]) == ERROR) ||
					(close(this->_pair_output[0]) == ERROR))
					throw ErrorHandler(__FILE__, __func__, __LINE__, "close file descriptor error");
				if (execve(this->_arg[0], this->_arg, this->_env) == ERROR)
					throw ErrorHandler(__FILE__, __func__, __LINE__, "execve file descriptor error");
			}
			else
			{
				if ((close(this->_pair_input[0]) == ERROR) ||
					(close(this->_pair_output[1]) == ERROR))
					throw ErrorHandler(__FILE__, __func__, __LINE__, "close2 file descriptor error");
			}
			if (_fld == 0)
			{
				close(this->_pair_input[1]);
				this->_pair_input[1] = 0;
			}
		}
};

#endif