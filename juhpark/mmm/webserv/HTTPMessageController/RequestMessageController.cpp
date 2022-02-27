#include "RequestMessageController.hpp"

std::string		RequestMessage::getMethod()				{ return method; }
std::string		RequestMessage::getRequestTarget()		{ return request_target; }
std::string		RequestMessage::getUriDir()				{ return uri_dir; }
std::string		RequestMessage::getUriFile()			{ return uri_file; }
std::string		RequestMessage::getQueryString()		{ return query_string; }
double			RequestMessage::getHttpVersion()		{ return http_version; }
bool			RequestMessage::getIsCGI()				{ return isCGI; }
void			RequestMessage::setIsCGI(bool flag)		{ this->isCGI = flag; }

void	RequestMessage::printVar() {
	std::cout << __func__ << std::endl;
	std::cout << "method : " << method << std::endl;
	std::cout << "request_target : " << request_target << std::endl;
	std::cout << "uri_dir : " << uri_dir << std::endl;
	std::cout << "uri_file : " << uri_file << std::endl;
	std::cout << "query_string : " << query_string << std::endl;
	std::cout << "isCGI : " << isCGI << std::endl;
	std::cout << "isHTTP : " << isHTTP << std::endl;
	std::cout << "http_version : " << http_version << std::endl;
	std::cout << "----------------------" << std::endl;
}

		// // reset R equest message
void					RequestMessage::resetMessage() {
	start_line = "";
	header_field.clear();
	message_body = "";
	method = "";
	request_target = "";
	uri_dir = "";
	uri_file = "";
	isHTTP = "";
	http_version = 0;
	return ;
}

		// HTTP Method parsing 및 에러 처리
void					RequestMessage::parseMethod(int* start, int* end, std::string& message) {
	*start = 0;
	*end = message.find(' ');
	if (*end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request Message");
	this->method = message.substr(*start, *end);;
	if (method.compare("GET") == 0 ||
			method.compare("POST") == 0 ||
			method.compare("PUT") == 0 ||
			method.compare("PATCH") == 0 ||
			method.compare("DELETE") == 0 ||
			method.compare("HEAD") == 0 ||
			method.compare("CONNECT") == 0 ||
			method.compare("OPTIONS") == 0 ||
			method.compare("TRACE") == 0)
		return ;
	throw ErrorHandler(__FILE__, __func__, __LINE__, "HTTP Method parsing error in request message");
}

		// HTTP uri parsing 및 에러 처리
void					RequestMessage::parseTarget(int* start, int* end, std::string& message) {
	*start = *end + 1;
	*end = message.find(' ', *start);
	if (*end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no URI in Request Message");
	this->request_target = message.substr(*start, *end - *start);
	if (request_target.length() > 0 && request_target[0] == '/') {
		int dir_pos = request_target.find_last_of("/");
		int file_pos = request_target.find_last_of(".");
		size_t	query_pos = request_target.find_last_of("?");
		if (dir_pos < file_pos) {
			this->uri_dir = request_target.substr(0, dir_pos);
			if (message.substr(file_pos, 3).compare("php") == 0) {
				this->isCGI = true;
				this->uri_file = request_target.substr(dir_pos + 1, query_pos - dir_pos - 1);
				this->query_string = request_target.substr(query_pos + 1);
			}
			else {
				this->isCGI = false;
				this->uri_file = request_target.substr(dir_pos + 1);
			}
		}
		else {
			this->isCGI = false;
			this->uri_dir = request_target;
			this->uri_file = config.getConfig("index");
		}
		return ;
	}
	throw ErrorHandler(__FILE__, __func__, __LINE__, "URI parsing error in request message");
}

		// HTTP protocol check 및 에러 처리
void					RequestMessage::parseIsHTTP(int* start, int* end, std::string& message) {
	*start = *end + 1;
	*end = message.find('/', *start);
	if (*end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol information in Request Message");
	this->isHTTP = message.substr(*start, *end - *start + 1);

	if (isHTTP.compare("HTTP/") == 0)
		return ;
	throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
}

// 		// TODO: double 타입의 값을 소수점 첫째 자리까지만 들어오도록 수정할 것
// 		// HTTP version check 및 에러 처리
void					RequestMessage::parseHTTPVersion(int* start, int* end, std::string& message) {
	*start = *end + 1;
	*end = message.find("\r\n", *start);
	if (*end == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol version in Request Message");
	http_version = atof(message.substr(*start).c_str());

	if (http_version == 1.0 ||
			http_version == 1.1 ||
			http_version == 2.0)
		return ;
	throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
}


// 		// start_line을 parsing하여 각 변수에 담는 함수
// 		// start_line의 데이터는 필수이므로 각 데이터에 대한 예외 처리
// 		// header_field parsing을 위해 start_line 이후 문자의 인덱스 반환
int						RequestMessage::parseStartLine(std::string& message) {
	int	start, end;

	this->parseMethod(&start, &end, message);
	this->parseTarget(&start, &end, message);
	this->parseIsHTTP(&start, &end, message);
	this->parseHTTPVersion(&start, &end, message);
    //this->parseContentLength(message);
	return (start + 5);
}

		// TODO: POST의 경우 message body도 parsing 필요함
		// Request Message parsing
		// start line, header field
void					RequestMessage::parsingRequestMessage(int fd, std::string& message) {
	int n;
	if ((n = this->parseStartLine(message)) == -1) {
		close(fd);
		throw ErrorHandler(__FILE__, __func__, __LINE__, "Start line parsing error in Request message");
	}

	n = this->parseHeaderField(message, n);

	// content length 헤더가 있는지 판별하여 body가 있는지 확인한다.		// content length 만큼만 읽고 response message 작성(write) 으로 바로 넘어가도록 작성해야한다.
	// client가 data를 계속 보내지 않고 있었을 때를 대비하여 timeout 을 설정 하여야 한다. (config에 기재되어있음)
	// cgi 라면 cgi.getInputPair() 로 fd 를 받아서 처리
	/*
	if (this->isCGI) {
		cgi.setEnvp(this);
		cgi.run();
		if (header_field.find("Content-Length") != header_field.end() && header_field["Content-Length"] != "0") {
			std::cout << "CGI DATA : " << (message.c_str() + n) << std::endl;
			write(cgi.getInputPair(), message.c_str() + n, message.length() - n);
		}
		char buf[1024];
		read(cgi.getOutputPair(), buf, 1024);
		std::cout << "buf : " << buf << std::endl;
	}
	*/
	printVar();
	return ;
}
