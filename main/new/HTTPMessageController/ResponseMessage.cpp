#include "ResponseMessage.hpp"
#include "MIMEController.hpp"
#include <fstream>

extern MIMEController mime;

ResponseMessage::ResponseMessage(HTTPData* _data) {
	this->data = _data;
	this->message = "";
	this->start_line = "";
	this->header_field = "";
	this->message_body = "";
	this->statusMessagePath = "./../setting/status_code.txt";
}

std::string ResponseMessage::setStatusMessage(std::string status_code) {
	std::string	rtn = "", read;
	int start, end;
	std::ifstream fileRead(this->statusMessagePath.c_str());
	while (getline(fileRead, read)) {
		start = 0;
		end = read.find(": ");
		if (status_code.compare(read.substr(start, end - start)) == 0) {
			start = end + 2;
			end = read.find("\n");
			rtn = read.substr(start, end - start);
			break;
		}
	}
	return (rtn);
}

void	ResponseMessage::setStartLine() {
	this->start_line += "HTTP/";
	// add var http_version on HTTPData class
	// this->start_line += this->data->http_version;
	this->start_line += "1.1";
	this->start_line += " ";
	// add var status_code on HTTPData class
	// this->start_line += this->data->status_code;
	this->start_line += "200";
	this->start_line += " ";
	this->start_line += setStatusMessage("200"); //this->data->status_code;
	return ;
}

void	ResponseMessage::setHeaderField() {
	if (this->data->isCGI == true) {
		// CGI의 반환값으로 오는 header-field 추가
	}
	else {
		// TODO
		// content-type을 지정해주기 위해서 request message의 uri중 파일 확장자가 필요
		//this->header_field += ("Content-Type: " + mime.getMIME(this->data->extension));
		this->header_field += ("Content-Length: " + std::to_string(this->message_body.length()));
	}
	this->header_field += "Accept-Ranges: bytes";
	return ;
}

void	ResponseMessage::setMessageBody() {
	if (this->data->isCGI == true) {
		// TODO
		// cgi pipe로부터 read하여 message_body에 저장
	}
	else {
		// TODO
		// uri 가 절대경로인지, 상대경로인지 확인할 필요 있음.
		// 상대경로의 경우, 앞에 ./로 시작하게 수정할 것
		std::ifstream	file(this->data->uri_dir);
		std::string		line;

		if (file.is_open()) {
			file.seekg(0, std::ios::end);
			int size = file.tellg();
			line.resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&line[0], size);
			this->message_body = line;
		}
		while (std::getline(file, line)) {
			this->message_body = this->message_body + line + "\n";
		}
	}
	return ;
}

void		ResponseMessage::setResponseMessage() {
	this->message += (this->start_line + "\r\n");
	this->message += (this->header_field + "\r\n\r\n");
	this->message += (this->message_body);
	return ;
}