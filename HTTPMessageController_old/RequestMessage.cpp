#include "RequestMessage.hpp"

RequestMessage::RequestMessage(HTTPData* _data) {
	this->data = _data;
	this->parsing_pointer = 0;
	this->message = "";
	this->seq = START_LINE;
}

int				RequestMessage::setError(int status_code) {
	this->data->is_error = true;
	this->data->status_code = status_code;
	return (ERROR);
}

void			RequestMessage::setMessage(char* buffer) {
	std::string	temp(buffer);
	this->message += temp;
	return ;
}

void			RequestMessage::resetMessage() {
	this->message = this->message.substr(this->parsing_pointer);
	this->parsing_pointer = 0;
	return ;
}

std::string		RequestMessage::getMessage(void) {
	return (this->message);
}

/** Header **/

// message 내에 start line이 모두 담겨져있는지 확인하는 메소드
// start line 파싱이 가능하면 true, 불가능하면 false 반환


int				RequestMessage::parsingRequestMessage() {
	if (this->seq == START_LINE) {
		if (int(this->message.find("\r\n")) != -1) {
			if (parseStartLine(this->message) == ERROR)
				return (ERROR);
			resetMessage();
			this->seq = HEADER_FIELD;
		}
	}
	if (this->seq == HEADER_FIELD) {
		if (int(this->message.find("\r\n\r\n", 0)) != -1) {
			if (parseHeaderField(this->message) == ERROR)
				return (ERROR);
			resetMessage();
			this->seq = MESSAGE_BODY;
		}
	}
	if (this->seq == MESSAGE_BODY) {
		if (data->header_field.find("Content-Length") != data->header_field.end() &&
			data->header_field["Content-Length"] != "0") {
			if (int(this->message.length()) == atoi(data->header_field["Content-Length"].c_str())) {
				parseMessageBody(this->message);
				this->seq = FINISH_PARSE;
			}
		}
		else
			this->seq = FINISH_PARSE;
	}
	return (this->seq);
}

int				RequestMessage::checkAutoIndex(std::string _root, std::string _path)
{
	std::string
		_abs_path =  _root + _path,
		_abs_root = _root;
	_abs_path = FileController::toAbsPath(_abs_path);
	_abs_root = FileController::toAbsPath(_abs_root);
		
	if (FileController::checkType(_abs_path) != FileController::DIRECTORY)
		return (-1);
	else if (_config._http._server[1].findLocationIndexByDir(_path) != -1 &&
			 _config._http._server[1]._location[
				 _config._http._server[1].findLocationIndexByDir(_path)
				]._dir_map["autoindex"] == "on")
		return (1);
	else if (_config._http._server[1].findLocationIndexByDir(_path) != -1 &&
			 _config._http._server[1]._location[
				 _config._http._server[1].findLocationIndexByDir(_path)
				]._dir_map["autoindex"] == "off")
		return (-1);
	else
	{
		if (_abs_path.find_last_of("/") == _abs_path.length())
			_abs_path = _abs_path.substr(0, _abs_path.length() - 1);
		if (_abs_path == _abs_root)
			return (-1);
		else
			return (checkAutoIndex(_root, FileController::getPrePath(_path)));
	}
}

// TODO
// 접근하는 경로가 파일이고, 해당 파일을 찾을 수 없는 경우 -> 404 ERROR
// 접근하는 경로가 파일이고, 해당 파일을 열 수 없는 경우 -> 403 ERROR
// 접근하는 경로가 디렉토리일 때, 오토인덱스 on이고, 인덱스 파일이 없는 경우 -> this->data->is_autoindex = true;
// 접근하는 경로가 디렉토리일 때, 오토인덱스 off이고, 인덱스 파일이 있는 경우 -> nginx 비교
// 접근하는 경로가 디렉토리일 때, 오토인덱스 off이고, 인덱스 파일이 없는 경우 -> nginx 비교
int				RequestMessage::parseStartLine(std::string &msg) {
	int	start, end;
	if (this->parseMethod(start, end, msg) == ERROR)
		return (ERROR);
	if (this->parseTarget(start, end, msg) == ERROR)
		return (ERROR);
	this->parseHttpVersion(start, end, msg);
	this->parsing_pointer = start + 2;
	/*
	if (TODO 조건에 따라 에러인 경우)
		return (setError(해당 에러 번호));
	*/
	return (START_LINE);
}

// TODO
// nginx.conf 에서 accepted_method에 해당하지 않는 경우, 405 에러 반환
// accepted_method에 GET, POST는 명시되지 않아도 적용 가능해야 함.
int				RequestMessage::parseMethod(int &start, int &end, std::string &msg) {
	start = 0;
	end = msg.find(' ');
	if (end == -1)
		return (setError(400));
	data->method = msg.substr(start, end);
	if (data->method.compare("GET") != 0 && data->method.compare("POST") != 0 && data->method.compare("DELETE") != 0)
		return (setError(400));
	/*
	if (nginx.conf 내 accepted_method가 있는 경우) {
		if (현재 HTTP method가 accepted_method와 매칭되지 않는 경우)
			return (setError(405));
	}
	*/
	return (START_LINE);
}

int				RequestMessage::parseTarget(int &start, int &end, std::string &msg) {
	std::string target;
	start = end + 1;
	end = msg.find(' ', start);
	if (end == -1)
		return (setError(400));

	target = msg.substr(start, end - start);

	int query_pos = int(target.find_last_of("?"));
	if (query_pos != -1) {
		data->query_string = target.substr(query_pos + 1);
		target = target.substr(0, target.length() - data->query_string.length() - 1);
	}
	int	extension_pos = target.find_last_of(".");
	if (extension_pos != -1) {
		data->file_extension = target.substr(extension_pos + 1);
		int	file_pos = target.find_last_of("/");
		data->uri_file = target.substr(file_pos + 1);
		target = target.substr(0, target.length() - data->uri_file.length());
	}
	data->uri_dir = target;
	this->parsing_pointer = end + 2;
	return (START_LINE);
}

void						RequestMessage::checkIsCgi(void) {
	for (int i = 0; i < int(_config._http._server[this->data->server_block]._location.size()); i++) {
		std::string temp = _config._http._server[this->data->server_block]._location[i]._location;
		temp = temp.substr(1, temp.length() - 2);
		if (this->data->file_extension.compare(temp) == 0) {
			this->data->is_cgi = true;
			this->data->cgi_pass = _config._http._server[this->data->server_block]._location[i]._dir_map["cgi_pass"];
			this->data->cgi_extension = temp;
			break;
		}
	}
}

std::vector<std::string>	RequestMessage::checkURIDIR(void) {
	std::vector<NginxConfig::LocationBlock> location = _config._http._server[data->server_block]._location;
	std::vector<NginxConfig::LocationBlock>::iterator it = location.begin();
	for (it = location.begin(); it != location.end(); it++) {
		if (it->_location.compare(data->uri_dir) == 0) {
			return (it->_index);
		}
	}
	return (_config._http._server[data->server_block]._index);
}

std::vector<std::string>	RequestMessage::checkErrorPage(void) {
	std::vector<NginxConfig::LocationBlock> location = _config._http._server[data->server_block]._location;
	std::vector<NginxConfig::LocationBlock>::iterator it = location.begin();
	for (it = location.begin(); it != location.end(); it++) {
		if (it->_location.compare(data->uri_dir) == 0) {
			return (it->_error_page);
		}
	}
	return (_config._http._server[data->server_block]._error_page);
}

std::string					RequestMessage::getErrorPage(std::vector<std::string> error_page, std::string root) {
	std::vector<std::string>::iterator pagePos = find(error_page.begin(), error_page.end(), std::to_string(data->status_code));
	if (pagePos != error_page.end()) {
		while (pagePos != error_page.end()) {
			int finder = (*pagePos).find_last_of(".html");
			if (finder != -1 && access((root + data->uri_dir + *pagePos).c_str(), F_OK) == 0) {
				return (*pagePos);
			}
			pagePos++;
		}
		data->uri_dir = "/";
		return ("defaul_error.html");
	}
	else {
		data->uri_dir = "/";
		return ("defaul_error.html");
	}
}

// 에러 페이지 띄워주기 설정
void	RequestMessage::checkTarget(void) {
	std::map<std::string, std::string>::iterator	rootFinder;
	rootFinder = _config._http._server[1]._dir_map.find("root");
	std::string	root;
	// default root 값 변경해야함
	// root값이 없다는 것을 알려주여야 status 코드를 띄울 수 있음 (304)
	if (rootFinder == _config._http._server[this->data->server_block]._dir_map.end()) {
		root = "./static_html";
		std::cout << "여기서 default root 값을 넣어주여야 함!" << std::endl;
	}

	root = _config._http._server[this->data->server_block]._dir_map["root"];
	this->data->url_directory = data->uri_dir;

	std::vector<std::string> index = checkURIDIR();
	std::vector<std::string>::iterator it;

	std::vector<std::string> error_page = checkErrorPage();
	// index 자체가 없을 때
	// root 경로에  default index.html을 띄워준다.
	// index.html이 없는 경우에는 403

	if (index.empty()) {
		std::string	filePath = root + data->uri_dir + "index.html";
		if (access(filePath.c_str(), F_OK) == 0) {
			data->uri_file = "index.html";
			data->file_extension = "html";
			data->status_code = 304;
			this->has_index = true;
			return ;
		} else {
			data->uri_file = getErrorPage(error_page, root);
			data->file_extension = "html";
			data->status_code = 403;
			this->has_index = true;
			return ;
		}
	}

	for(it = index.begin(); it != index.end(); it++) {
		std::string	filePath = root + data->uri_dir + "/" + *it;
		if (access(filePath.c_str(), F_OK) == 0) {
			data->file_extension = (*it).substr((*it).find_last_of('.') + 1);
			data->uri_file = *it;
			data->status_code = 200;
			this->has_index = true;
			return ;
		}
	}
	if (it == index.end()) {
		data->uri_file = getErrorPage(error_page, root);
		data->file_extension = "html";
		data->status_code = 403;
		this->has_index = false;
	}
}

void	RequestMessage::parseHttpVersion (int &start, int &end, std::string &msg) {
	start = end + 1;
	std::string http_check = msg.substr(start);
	if (http_check.compare("HTTP/1.1") == 0) {
		this->data->http_version = 1.1;
		start += http_check.length();
		return ;
	}
	else
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Not HTTP 1.1");
}

void	RequestMessage::printStartLine(void) {
	std::cout << "[REQUEST MSG - START LINE]" << std::endl;
	std::cout << "Method : " << data->method << std::endl;
	std::cout << "uri_dir : " << data->uri_dir << std::endl;
	std::cout << "uri_file : " << data->uri_file << std::endl;
	std::cout << "query_string : " << data->query_string << std::endl;
	std::cout << "file_extension : " << data->file_extension << std::endl;
	std::cout << "http_version : " << data->http_version << std::endl;
	std::cout << "is_cgi : " << data->is_cgi << std::endl;
}

		/** Header Field **/
int		RequestMessage::parseHeaderField(std::string &msg) {
	int start = this->parsing_pointer, end;
	std::string key, value;
	// header_field가 없는 경우
	if ((int)msg.length() == start) {
		this->parsing_pointer = start + 2;
		return (HEADER_FIELD);
	}
	while (true) {
		end = msg.find(':', start);
		key = msg.substr(start, end - start);
		start = end + 1;
		if (msg.at(end + 1) == ' ')
			start++;
		end = msg.find("\r\n", start);
		value = msg.substr(start, end - start);
		data->header_field.insert(std::pair<std::string, std::string>(key, value));
		start = end + 2;
		// header_field가 끝나는 경우 (CRLF)
		if (msg.at(start) == '\r' && msg.at(start + 1) == '\n')
			break;
	}
	if (this->data->is_cgi == true && data->header_field["Content-Length"].empty() == false) {
		int	content_length = atoi(data->header_field["Content-Length"].c_str());
		if (content_length > this->data->client_body_size)
			return (setError(413));
	}
	this->parsing_pointer = start + 2;
	return (HEADER_FIELD);
}

void	RequestMessage::printHeaderField(void) {
	std::cout << "[REQUEST MSG - HEADER FIELD]" << std::endl;
	std::map<std::string, std::string>::iterator it;

	for (it = data->header_field.begin(); it != data->header_field.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}

/** Body **/
void	RequestMessage::parseMessageBody(std::string &msg) {
	int start = this->parsing_pointer;

	data->message_body = msg.substr(start);
	if (data->is_cgi) {
	//	data->message_body += data->CGI_read;
	//	write(cgi->getInputPair(), data->message_body.c_str(), data->message_body.length());
	}
}

void	RequestMessage::printBody(void) {
	std::cout << "[REQUEST MSG - BODY]" << std::endl;
	std::cout << data->message_body << std::endl;
}

void
	RequestMessage::set413ErrorPage(
		void)
{
	data->uri_file = "413.html";
	data->file_extension = "html";
	data->status_code = 413;
	data->uri_dir = "/";
	data->is_cgi = false;
}