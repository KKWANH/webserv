#include "CGIProcess.hpp"
#include "RequestMessageController.hpp"
#include <iostream>

char**	CGIProcess::generateEnvp(std::map<std::string, std::string> env)
{
	char	**envp = new char*[env.size() + 1];
	int i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = env.begin(); it != env.end(); ++it)
	{
		envp[i] = new char[(it->first.length() + it->second.length() + 2)];
		strcpy(envp[i], it->first.c_str());
		strcat(envp[i], "=");
		strcat(envp[i], it->second.c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

void	CGIProcess::setCGIarg() {
	this->_arg = new char*[4];

	this->_arg[0] = new char[22];
	strcpy(_arg[0], "./cgiBinary/php-cgi");  // cgi file 경로. config 파일에서 파싱해서 사용

	this->_arg[1] = new char[25];
	strcpy(_arg[1], "./cgiBinary/sample.php");  // 실행할 파일 경로. request message의 uri에서 파싱해서 사용

	this->_arg[2] = new char[10];
	strcpy(_arg[2], "var=1234");
	/*
	querystring 값
	if (requestMessage->_URIQueryString.empty())
		_arg[2] = NULL;
	else
		_arg[2] = const_cast<char*>(requestMessage->_URIQueryString.c_str());
	*/
	_arg[3] = NULL;
}

// 메소드, PHP 바이너리, PHP 파일 경로, 컨텐츠 길이...?
//요청 메세지를 파싱해서 다른 곳에다가 담아서 전달을 해보는 것도 나을듯
void CGIProcess::setEnvp(void *requestMessage)
{
	RequestMessage *reqMsg = static_cast<RequestMessage*>(requestMessage);
	std::map<std::string, std::string> _envMap;

	// local 환경변수에 이미 존재하는 아이들
	_envMap[std::string("USER")] = std::string(std::getenv("USER"));
	_envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
	_envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
	_envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
	// parsing으로 가져온 아이들
	_envMap[std::string("REQUEST_METHOD")] = reqMsg->getMethod();
	// _envMap[std::string("PATH_INFO")] = requestMessage->getUriDir(); // /data/index.html
	_envMap[std::string("PATH_INFO")] = std::string("/index.html");
	_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
	_envMap[std::string("REQUEST_SCHEME")] = reqMsg->getMethod();
	_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
	_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

	// _envMap[std::string("CONTENT_TYPE")] = requestMessage->_reqContentType;
	_envMap[std::string("CONTENT_TYPE")] = mime.getMIME("php");
	// _envMap[std::string("CONTENT_LENGTH")] = requestMessage->_reqContentLength;
	_envMap[std::string("CONTENT_LENGTH")] = reqMsg->getHeaderField("Content-Length");

	// _envMap[std::string("REMOTE_ADDR")] = requestMessage->_clientIP; // server socket addr
	// _envMap[std::string("REMOTE_PORT")] = requestMessage->_clientPort; // client port
	// _envMap[std::string("SERVER_ADDR")] = requestMessage->_hostIP; // client socket addr
	// _envMap[std::string("SERVER_PORT")] = requestMessage->_hostPort; // host port
	_envMap[std::string("QUERY_STRING")] = reqMsg->getQueryString();
	_envMap[std::string("SCRIPT_NAME")] = reqMsg->getUriFile();

	//여기는 binary가 있는 루트경로 + binary파일 이름
	//그럼 여걸 어캐 어디서 파싱해서 넣지
	// _envMap[std::string("REQUEST_URI")] = requestMessage->_originURI;
//	_envMap[std::string("DOCUMENT_ROOT")] = "./cgiBinary";
//	_envMap[std::string("REQUEST_URI")] = "./cgiBinary/" + reqMsg->getUriFile();
//	_envMap[std::string("DOCUMENT_URI")] = "./cgiBinary/" + reqMsg->getUriFile();
	// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
//	_envMap[std::string("SCRIPT_FILENAME")] = "./cgiBinary/" + reqMsg->getUriFile();
	_envMap[std::string("DOCUMENT_ROOT")] = "./static_html";
	_envMap[std::string("REQUEST_URI")] = "./static_html" + reqMsg->getQueryString();
	_envMap[std::string("DOCUMENT_URI")] = "./static_html" + reqMsg->getQueryString();
	// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
	_envMap[std::string("SCRIPT_FILENAME")] = std::string(std::getenv("PWD")) + "/static_html" + reqMsg->getUriFile();

		// for (std::map<std::string, std::string>::iterator iter = requestMessage->_HTTPCGIENV.begin(); iter != requestMessage->_HTTPCGIENV.end(); iter++) {
		//	 _envMap[iter->first] = iter->second;
		// } // CGI binary 파일의 헤더 (HTTP_)
		// request의 헤더가 cgi 의 환경변수로 들어가야 한다.
	_env = generateEnvp(_envMap);
	
	//환경변수가 잘들갔나
	for (std::map<std::string, std::string>::iterator iter = _envMap.begin(); iter != _envMap.end(); iter++) {
		std::cout << iter->first << " = " << iter->second << std::endl;
	}

	setCGIarg();
}

CGIProcess::~CGIProcess()
{
	int status;
	if (_pid > 0) {
		waitpid(_pid, &status, WNOHANG);
		if (status & 0177) {
			kill(_pid, SIGTERM);
		}
	}
	// if (getInputPair() > 0 && close(getInputPair()) == -1) {
	// 	 throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error3.");
	// }
	// if (getOutputPair() > 0 && close(getOutputPair()) == -1) {
	// 	 throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error.");
	// }
	// if (_env != NULL) {
	// 	int i = 0;
	// 	while (_env[i] != NULL) {
	// 		delete _env[i];
	// 		i++;
	// 	}
	// 	delete _env;
	// }
}

int& CGIProcess::getInputPair(void)
{
	return (_inputPair[1]);
}

int& CGIProcess::getOutputPair(void)
{
	return (_outputPair[0]);
}

void CGIProcess::run(void)
{
	if (pipe(this->_inputPair) == -1 || pipe(this->_outputPair) == -1) {
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Pipe Making Erro.r");
	}
	if ((_pid = fork()) < 0) {
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Process Making Error.");
	}
	if (_pid == 0) {
		if ((dup2(this->_inputPair[0], STDIN_FILENO) == -1) || (dup2(this->_outputPair[1], STDOUT_FILENO) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "duplicate File Descriptor Error.");
		}
		if ((close(this->_inputPair[1]) == -1) || (close(this->_outputPair[0]) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error1.");
		}
	//	std::cout << "arg : " << _arg[0] << "env : " << _env[0] << std::endl;
		if (execve(this->_arg[0], _arg, _env) == -1) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "execve File Descriptor Error.");
		}
	} else {
		if ((close(this->_inputPair[0]) == -1) || (close(this->_outputPair[1]) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error2.");
		}
	}
}
