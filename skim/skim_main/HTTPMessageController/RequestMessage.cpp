#include "RequestMessage.hpp"

// nginx 설정 파일에 띄워줄 파일에 대한 설정이 없다면 그것에 대해 어떻게 처리할 것인지
// request message에서 status 코드를 저장해야 한다.
// 파일이 없을 때 not found 처리


RequestMessage::RequestMessage(HTTPData* _data) : data(_data), parsing_pointer(0), message(""), seq(START_LINE) { }

void	RequestMessage::setMessage(char* buffer) {
	std::string	temp(buffer);
	this->message += temp;
	// std::cout << "\n\nmessage\n" << message << "\n\n";
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
	printStartLine();
	printHeaderField();
	printBody();
	std::cout << "\n\n";
	return (this->seq);
}

void	RequestMessage::parseStartLine(std::string &msg) {
	int	start, end;
	this->parseMethod(start, end, msg);
	this->parseTarget(start, end, msg);
	this->parseHttpVersion(start, end, msg);
	this->parsing_pointer = start + 5;
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
	int	extension_pos = target.find_last_of(".");
	this->data->isCGI = false;
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
				break;
			}
		}
	}

	data->uri_dir = target;
	this->parsing_pointer = end + 2;
	checkTarget();
	resetMessage();
}

// url에서 특정 파일이 없을 경우 404 Not Found ?
void	RequestMessage::checkTarget(void) {
	// location의 root가 아닌 serverBlock의 root는 어디있지..?
	data->uri_dir = _config._http._server[this->data->server_block]._location[0]._inherit_data._root + "/";
	std::vector<std::string>::iterator it = _config._http._server[this->data->server_block]._index.begin();
	for(; it != _config._http._server[this->data->server_block]._index.end(); it++) {
		std::ifstream	file(data->uri_dir + *it);
		if (file.is_open()) {
			data->uri_dir = data->uri_dir + *it;
			file.close();
			data->status_code = "200";
			break;
		} else {
			file.close();
		}
	}
	if (it == _config._http._server[this->data->server_block]._index.end())
		data->status_code = "403";
}

void	RequestMessage::parseHttpVersion(int &start, int &end, std::string &msg) {
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
