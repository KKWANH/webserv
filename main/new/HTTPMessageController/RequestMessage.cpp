#include "RequestMessage.hpp"

RequestMessage::RequestMessage(HTTPData* _data) : data(_data), parsing_pointer(0), message(""), seq(START_LINE) { }

void	RequestMessage::setMessage(char* buffer) {
	std::string	temp(buffer);
	this->message += temp;
	return ;
}

void	RequestMessage::resetMessage() {
	this->message = this->message.substr(this->parsing_pointer);
	return ;
}

/** Header **/

// message 내에 start line이 모두 담겨져있는지 확인하는 메소드
// start line 파싱이 가능하면 true, 불가능하면 false 반환
int		RequestMessage::parsingRequestMessage() {
	if (this->seq == START_LINE) {
		int	start_line_pos = int(this->message.find("\r\n", 0));
		std::string start_line_msg = this->message.substr(0, start_line_pos);
		if (start_line_pos != -1) {
			parseStartLine(start_line_msg);
			resetMessage();
			this->seq = HEADER_FIELD;
		}
		if (this->data->isCGI == true) {
			std::cout << "CGIII" << std::endl;
		//	CGIProcess cgi(this->data);
		//	cgi.run();
		}
	}
	if (this->seq == HEADER_FIELD) {
		if (int(this->message.find("\r\n\r\n", 0)) != -1) {
			parseHeaderField(this->message);
			resetMessage();
			this->seq = MESSAGE_BODY;
		}
	}
	if (this->seq == MESSAGE_BODY) {
		if (data->header_field.find("Content-Length") != data->header_field.end() &&
			data->header_field["Content-Length"] != "0") {
			if (int(this->message.length()) == atoi(data->header_field["Content-Length"].c_str())) {
				std::cout << "MESSAGE BODY PARSING" << std::endl;
				parseMessageBody(this->message);
				this->seq = FINISH_PARSE;
			}			
		}
		else
			this->seq = FINISH_PARSE;
	}
	return (this->seq);
}

void	RequestMessage::parseStartLine(std::string &msg) {
	int	start, end;
	this->parseMethod(start, end, msg);
	this->parseTarget(start, end, msg);
	this->parseHttpVersion(start, end, msg);
	this->parsing_pointer = start + 5;
	
	if (this->data->uri_file.compare("") == 0) {
		checkTarget();
	}
}

void	RequestMessage::parseMethod(int &start, int &end, std::string &msg) {
	start = 0;
	end = msg.find(' ');
	if (end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request msg");
		data->method = msg.substr(start, end);
		if (data->method.compare("GET") == 0 ||
			data->method.compare("POST") == 0 ||
			data->method.compare("PUT") == 0 ||
			data->method.compare("PATCH") == 0 ||
			data->method.compare("DELETE") == 0 ||
			data->method.compare("HEAD") == 0 ||
			data->method.compare("CONNECT") == 0 ||
			data->method.compare("OPTIONS") == 0 ||
			data->method.compare("TRACE") == 0)
		return ;
	throw ErrorHandler(__FILE__, __func__, __LINE__, "HTTP Method parsing error in request msg");
}

void	RequestMessage::parseTarget(int &start, int &end, std::string &msg) {
	std::string target;
	start = end + 1;
	end = msg.find(' ', start);
	if (end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no URI in Request msg");
	target = msg.substr(start, end - start);

	int query_pos = int(target.find_last_of("?"));
	if (query_pos != -1) {
		data->query_string = target.substr(query_pos + 1);
		target = target.substr(0, target.length() - data->query_string.length() - 1);
		
	}
//	for (int i = 0; i < int(_config._http._server[this->data->server_block]._location.size()); i++)
//	{
//		std::cout << "what is this ? : " <<  _config._http._server[this->data->server_block]._location[i]._location << std::endl;
//	}
	for (int i = 0; i < int(_config._http._server[this->data->server_block]._location.size()); i++)
	{
		std::cout << "cgi_pass : " << _config._http._server[this->data->server_block]._location[i]._dir_map["cgi_pass"] << std::endl;
	}
	int	extension_pos = target.find_last_of(".");
	this->data->isCGI = false;
	this->data->CGI_root = "";
	if (extension_pos != -1) {
		data->file_extension = target.substr(extension_pos + 1);
		int	file_pos = target.find_last_of("/");
		data->uri_file = target.substr(file_pos + 1);
		target = target.substr(0, target.length() - data->uri_file.length());
		for (int i = 0; i < int(_config._http._server[this->data->server_block]._location.size()); i++) {
			std::string temp = _config._http._server[this->data->server_block]._location[i]._location;
			temp = temp.substr(1, temp.length() - 2);
			if (this->data->file_extension.compare(temp) == 0) {
				this->data->isCGI = true;
				this->data->CGI_root = _config._http._server[this->data->server_block]._location[i]._dir_map["cgi_pass"];
				this->data->CGI_what = temp;
				//그럼 cgi를 찾았다면
				//어느 cgi인지(php냐 아님 python이냐)
				//그것도 여기서 찾아야 할것이고
				//그것에 대한 경로 파일도 찾아내는게 맞을듯
				break;
			}
		}
	}
	data->uri_dir = target;
	this->parsing_pointer = end + 2;
	resetMessage();
}

// location 블럭 별로 uri_dir의 index를 가져와야함
// 에러 페이지 띄워주기 설정
void	RequestMessage::checkTarget(void) {
	std::map<std::string, std::string>::iterator rootFinder;
	rootFinder = _config._http._server[this->data->server_block]._dir_map.find("root");

	// default root 값 변경해야함
	// root값이 없다는 것을 알려주여야 status 코드를 띄울 수 있음 (304)
	if (rootFinder == _config._http._server[this->data->server_block]._dir_map.end()) {
		std::cout << "여기서 default root 값을 넣어주여야 함!" << std::endl;
	}

	std::string root = _config._http._server[this->data->server_block]._dir_map["root"];
	// index 자체가 없을 때
	// root 경로에 index.html을 띄워준다.
	// index.html이 없는 경우에는 403
	if (_config._http._server[this->data->server_block]._index.empty()) {
		std::string	filePath = root + data->uri_dir + "index.html";
		if (access(filePath.c_str(), F_OK) == 0) {
			data->uri_file = "index.html";
			data->file_extension = "html";
			data->status_code = 304;
			return ;
		} else {
			data->status_code = 403;
			return ;
		}
	}

	std::vector<std::string>::iterator it = _config._http._server[data->server_block]._index.begin();
	for(; it != _config._http._server[data->server_block]._index.end(); it++) {
		std::string	filePath = root + data->uri_dir + *it;
		if (access(filePath.c_str(), F_OK) == 0) {
			data->file_extension = (*it).substr((*it).find_last_of('.') + 1);
			data->uri_file = *it;
			data->status_code = 200;
			return ;
		}
	}
	if (it == _config._http._server[this->data->server_block]._index.end())
		data->status_code = 403;
}

void	RequestMessage::parseHttpVersion (int &start, int &end, std::string &msg) {
	start = end + 1;
	std::string httpCheck = msg.substr(start);
	if (httpCheck.compare("HTTP/1.1") == 0) {
		this->data->http_version = 1.1;
		start += httpCheck.length();
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
	std::cout << "isCGI : " << data->isCGI << std::endl;
}

		/** Header Field **/
void	RequestMessage::parseHeaderField(std::string &msg) {
	int start = this->parsing_pointer, end;
	std::string key, value;
	// header_field가 없는 경우
	if ((int)msg.length() == start) {
		this->parsing_pointer = start + 2;
		return ;
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
	this->parsing_pointer = start + 2;
	return ;
}

void	RequestMessage::printHeaderField(void) {
	std::cout << "[REQUEST MSG - HEADER FIELD]" << std::endl;
	std::map<std::string, std::string>::iterator it;

	for (it = data->header_field.begin(); it != data->header_field.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}

/** Body **/
void	RequestMessage::parseMessageBody(std::string &msg) {
	int start = this->parsing_pointer, end;
	std::string key, value;
	
	end = msg.find("\r\n", start);
	data->message_body = msg.substr(start, end - start);
	//if (data->isCGI)
		//write(cgi.getInputPair(), data->message_body.c_str(), data->message_body.length());
}

void	RequestMessage::printBody(void) {
	std::cout << "[REQUEST MSG - BODY]" << std::endl;
	std::cout << data->message_body << std::endl;
}
