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

std::string	ResponseMessage::getMessage(void) {
	return (this->message);
}

std::string ResponseMessage::setStatusMessage(std::string status_code) {
	std::string	rtn, read, temp;
	rtn = "";
	int start, end;
	std::ifstream fileRead(this->statusMessagePath.c_str());

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
		// FIXME
		// CGI의 경우, Chunked data를 받아야 함.
		//this->header_field += "Transfer-Encoding: chunked\r\n";
		//this->header_field += "Content-Length: 51\r\n";
	}
	else {
		// TODO
		// content-type을 지정해주기 위해서 request message의 uri중 파일 확장자가 필요
		this->header_field += ("Content-Type: " + _mime.getMIME(this->data->file_extension) + "\r\n");
		std::string path = _config._http._server[this->data->server_block]._dir_map["root"] + this->data->uri_dir + this->data->uri_file;
		std::stringstream ss;
		ss << FileController::getFileSize(path);
		this->header_field += ("Content-Length: " + ss.str() + "\r\n");
	}
	this->header_field += "Accept-Ranges: bytes\r\n";
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
	this->message += this->header_field;
	if (this->data->isCGI == false)
		this->message += "\r\n";
	return (0);
}