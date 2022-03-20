#include "ResponseMessage.hpp"
#include <fstream>
#include <sstream>
#include "ConfigBlocks.hpp"
#include "ConfigMime.hpp"
#include "FileController.hpp"

extern MimeConfig
	_mime;
extern NginxConfig::GlobalConfig
	_config;

ResponseMessage::ResponseMessage(HTTPData* _data) {
	this->data = _data;
	this->message = "";
	this->start_line = "";
	this->header_field = "";
	this->message_body = "";
	this->statusMessagePath = "./setting/status_code.txt";
}

std::string	returnRedirectMessage() {
	std::string
		redirectMessage = "HTTP/1.1 302\nLocation: ";
	redirectMessage += _config._http._server[1]._location[6]._rewrite[1];
	redirectMessage += "\nContent-Type: text/html;charset=UTF-8\nContent-Length: 0\r\n";
	return (redirectMessage);
}

void		ResponseMessage::resetMessage(int buf_size) {
	this->message = this->message.substr(buf_size);
}

void		ResponseMessage::printStartLine(void) {
	std::cout << this->start_line << std::endl;
}
void		ResponseMessage::printHeaderField(void) {
	std::cout << this->header_field << std::endl;
}
void		ResponseMessage::printMessageBody(void) {
	std::cout << this->message_body << std::endl;
}

std::string	ResponseMessage::getMessage(void) {
	return (this->message);
}

std::string ResponseMessage::setStatusMessage(std::string status_code) {
	std::string	rtn, read, temp;
	rtn = "";
	int start, end;
	std::ifstream fileRead(this->statusMessagePath.c_str());
	//std::cout << "Start set status code message" << std::endl;

	if (fileRead.peek() == std::ifstream::traits_type::eof())
		throw ErrorHandler(__FILE__, __func__, __LINE__, "status_code.txt is empty");
	while (getline(fileRead, read)) {
		start = 0;
		end = read.find(": ");
		temp = read.substr(start, end - start);
		if (status_code.compare(temp) == 0) {
			start = end + 2;
			end = read.find("\n");
			rtn = read.substr(start, end - start);
			break;
		}
	}
	return (rtn);
}

void	ResponseMessage::setStartLine() {
	std::stringstream	temp;
	std::string			str_status_code;

	// FIXME
	// 임시 status_code 값
	this->data->status_code = 200;
	temp << this->data->status_code;
	temp >> str_status_code;

	this->start_line += "HTTP/1.1 ";
	this->start_line += str_status_code;
	this->start_line += " ";
	this->start_line += setStatusMessage(str_status_code);
	return ;
}

void	ResponseMessage::setHeaderField() {
	if (this->data->isCGI == true) {
		// CGI의 반환값으로 오는 header-field 추가
	}
	else {
		// TODO
		// content-type을 지정해주기 위해서 request message의 uri중 파일 확장자가 필요
		this->header_field += ("Content-Type: " + _mime.getMIME(this->data->file_extension) + "\r\n");
		std::string path = _config._http._server[this->data->server_block]._dir_map["root"] + this->data->uri_dir + this->data->uri_file;
		std::stringstream ss;
		std::cout << FileController::getFileSize(path);
		this->header_field += ("Content-Length: " + ss.str() + "\r\n");
	}
	this->header_field += "Accept-Ranges: bytes\r\n";
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
		std::string		path = _config._http._server[this->data->server_block]._dir_map["root"] + this->data->uri_dir + this->data->uri_file;
		//std::cout << "[PATH] : " + path << std::endl;
		std::ifstream	file(path);
		std::string		line;

		if (file.is_open()) {
			file.seekg(0, std::ios::end);
			int size = file.tellg();
			line.resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&line[0], size);
			this->message_body = line;
			while (std::getline(file, line)) {
				this->message_body = this->message_body + line + "\n";
			}
		}
	}
	this->header_field += ("Content-Length: " + std::to_string(this->message_body.length()) + "\r\n");
	return ;
}

int			ResponseMessage::setResponseMessage(std::string _tmp_directory)
{
	for (int _idx=0; _idx<(int)(_config._http._server[1]._location.size()); _idx++)
	{
		if (_config._http._server[1]._location[_idx]._location == _tmp_directory &&
			(int)_config._http._server[1]._location[_idx]._rewrite.size() != 0 &&
			_config._http._server[1]._location[_idx]._rewrite[0] == _tmp_directory)
		{
			this->message += returnRedirectMessage();
			return (1);
		}
	}
	setStartLine();
	setHeaderField();
	this->message += (this->start_line + "\r\n");
	this->message += (this->header_field + "\r\n");
	return (0);
}