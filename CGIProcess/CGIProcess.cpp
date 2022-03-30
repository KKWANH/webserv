# include	"CGIProcess.hpp"

CGIProcess::CGIProcess(
		HTTPData*	data)
{
	setCGIArgv(data);
	setEnvp(data);
}

CGIProcess::~CGIProcess(
		void)
{
	int
		status;
	if (_pid > 0)
	{
		waitpid(_pid, &status, 0);
		if (status & 0177)
			kill(_pid, SIGTERM);
	}

	for (int i = 0; i < 3; i++)
		delete argv[i];

	for (int i = 0; i < env_size; i++)
		delete envp[i];
	delete envp;
}

char*
	CGIProcess::getFilePath(
		void)
{
	return (this->argv[0]);
}

char**
	CGIProcess::getArgv(
		void)
{
	return (this->argv);
}

char**
	CGIProcess::generateEnvp(
		std::map<std::string, std::string>	env)
{
	char**
		ret = new char*[env.size() + 1];
	int
		i = 0;
	std::map<std::string, std::string>::iterator
		it;

	env_size = env.size();
	for (it = env.begin(); it != env.end(); ++it)
	{
		ret[i] = new char[(it->first.length() + it->second.length() + 2)];
		strcpy(ret[i], it->first.c_str());
		strcat(ret[i], "=");
		strcat(ret[i], it->second.c_str());
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

void
	CGIProcess::setCGIArgv(
		HTTPData* data)
{
	// this->argv = new char**;

	this->argv[0] = new char[data->CGI_root.size() + 1];
	strcpy(this->argv[0], data->CGI_root.c_str());

	// TODO
	// 요청으로 들어온 경로를 절대경로로 넣어줄 것
	std::string path = _config._http._server[data->server_block]._dir_map["root"] + data->uri_dir + data->uri_file;
	path = FileController::toAbsPath(path);
	this->argv[1] = new char[path.length() + 1];
	strcpy(this->argv[1], path.c_str());
	// "/Users/hybae/Desktop/webserv/main/new/cgiBinary/sample.php"
	this->argv[2] = new char[data->query_string.size() + 1];
	strcpy(this->argv[2], data->query_string.c_str());

	this->argv[3] = NULL;
}

void
	CGIProcess::setEnvp(
		HTTPData* data)
{
	std::map<std::string, std::string>
		_envMap;

	//cgi루트에서 파일명만 뺴고 싶다면
	std::string
		root = data->CGI_root.substr(1, data->CGI_root.size());
	size_t
		start = root.find_last_of("/"),
		finish = root.find_last_of(root);

	only_file = root.substr(start + 1, finish - start);
	only_root = root.substr(0, start);

	_envMap[std::string("USER")] = std::string(std::getenv("USER"));
	_envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
	_envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
	_envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
	_envMap[std::string("REQUEST_METHOD")] = data->method;
	_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
	_envMap[std::string("REQUEST_SCHEME")] = data->method;
	_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
	_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");
	_envMap[std::string("CONTENT_TYPE")] = data->header_field["Content-Type"];
	_envMap[std::string("CONTENT_LENGTH")] = data->header_field["Content-Length"];
	_envMap[std::string("QUERY_STRING")] = data->query_string;

	_envMap[std::string("DOCUMENT_ROOT")] = only_root;
	_envMap[std::string("REQUEST_URI")] = data->uri_dir + data->uri_file; // 리퀘스트에 명시된 전체 주소가 들어가야 함
	_envMap[std::string("DOCUMENT_URI")] = data->uri_dir + data->uri_file; // 리퀘스트에 명시된 전체 주소가 들어가야 함
	// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
	// TODO
	// envp[1]과 동일
	std::string path = _config._http._server[data->server_block]._dir_map["root"] + data->uri_dir + data->uri_file;
	path = FileController::toAbsPath(path);
	_envMap[std::string("SCRIPT_FILENAME")] = path; // 실행하고자 하는 파일의 절대 경로가 들어가야 함.
	_envMap[std::string("SCRIPT_NAME")] = path;
	_envMap[std::string("PATH_INFO")] = data->uri_dir + data->uri_file;

	_envMap[std::string("REMOTE_ADDR")] = data->client_ip; // server socket addr
	_envMap[std::string("REMOTE_PORT")] = data->client_port; // serverSocketAddr.sin_port => string화
	_envMap[std::string("SERVER_ADDR")] = data->host_ip; // server socket addr
	_envMap[std::string("SERVER_PORT")] = data->host_port;

	// cookie
	std::map<std::string, std::string>::iterator iter = data->header_field.begin();
	for (iter = data->header_field.begin(); iter != data->header_field.end(); iter++) {
		std::string key = std::string("HTTP_") + iter->first;
		for (size_t i = 0; i < key.length(); i++) {
			if (key[i] >= 'a' && key[i] <= 'z') {
				key[i] = key[i] - ('a' - 'A');
			} else if (key[i] == '-') {
				key[i] = '_';
			}
		}
		_envMap[key] = iter->second;
	}
	envp = generateEnvp(_envMap);
}

int&
	CGIProcess::getInputPair(
		void)
{
	return (inputPair[1]);
}

int&
	CGIProcess::getOutputPair(
		void)
{
	return (outputPair[0]);
}

void
	CGIProcess::run(
		void)
{
	if (pipe(this->inputPair) == -1 || pipe(this->outputPair) == -1) {
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Pipe Making Error.");
	}
	//fork
	if ((_pid = fork()) < 0) {
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Process Making Error.");
	}
	//자식
	if (_pid == 0) {
		if ((dup2(this->inputPair[0], STDIN_FILENO) == -1) || (dup2(this->outputPair[1], STDOUT_FILENO) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "duplicate File Descriptor Error.");
		}
		if ((close(this->inputPair[1]) == -1) || (close(this->outputPair[0]) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error1.");
		}
		if (execve(this->argv[0], this->argv, envp) == -1) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "execve File Descriptor Error.");
		}
	} else {
		if ((close(this->inputPair[0]) == -1) || (close(this->outputPair[1]) == -1)) {
			throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error2.");
		}
	}
}