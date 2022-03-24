#include "ResponseMessage.hpp"

ResponseMessage::ResponseMessage(HTTPData* _tmp) {
	this->_data = _tmp;
	this->_message = "";
	this->_start_line = "";
	this->_header_field = "";
	this->_status_msg_path = "./setting/status_code.txt";
}

std::string	returnRedirectMessage() {
	std::string
		redirectMessage = "HTTP/1.1 302\nLocation: ";
	redirectMessage += _config._http._server[1]._location[6]._rewrite[1];
	redirectMessage += "\nContent-Type: text/html;charset=UTF-8\nContent-Length: 0\r\n";
	return (redirectMessage);
}

void		ResponseMessage::resetMessage(int buf_size) {
	this->_message = this->_message.substr(buf_size);
}

void		ResponseMessage::printStartLine(void) {
	std::cout << this->_start_line << std::endl;
}
void		ResponseMessage::printHeaderField(void) {
	std::cout << this->_header_field << std::endl;
}

std::string	ResponseMessage::getMessage(void) {
	return (this->_message);
}

std::string ResponseMessage::setStatusMessage(std::string status_code) {
	std::string	rtn, read, temp;
	rtn = "";
	int start, end;
	std::ifstream fileRead(this->_status_msg_path.c_str());

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
	this->_data->status_code = 200;
	temp << this->_data->status_code;
	temp >> str_status_code;

	this->_start_line += "HTTP/1.1 ";
	this->_start_line += str_status_code;
	this->_start_line += " ";
	this->_start_line += setStatusMessage(str_status_code);
	return ;
}

void	ResponseMessage::setHeaderField() {
	if (this->_data->isCGI == true) {
		// FIXME
		// CGI의 경우, Chunked _data를 받아야 함.
		//this->_header_field += "Transfer-Encoding: chunked\r\n";
		//this->_header_field += "Content-Length: 51\r\n";
	}
	else if (this->_data->is_autoindex == true)
	{

	}
	else {
		// TODO
		// content-type을 지정해주기 위해서 request message의 uri중 파일 확장자가 필요
		this->_header_field += ("Content-Type: " + _mime.getMIME(this->_data->file_extension) + "\r\n");
		std::string
			_path = _config._http._server[this->_data->server_block]._dir_map["root"] + this->_data->uri_dir + this->_data->uri_file;
		std::stringstream
			_strstream;
		_strstream << FileController::getFileSize(_path);
		this->_header_field += ("Content-Length: " + _strstream.str() + "\r\n");
	}
	this->_header_field += "Accept-Ranges: bytes\r\n";
	return ;
}

int	
	ResponseMessage::setResponseMessage(
		void)
{
	for (int _idx=0; _idx<(int)(_config._http._server[1]._location.size()); _idx++)
	{
		if (_config._http._server[1]._location[_idx]._location == this->_data->url_directory &&
			(int)_config._http._server[1]._location[_idx]._rewrite.size() != 0 &&
			_config._http._server[1]._location[_idx]._rewrite[0] == this->_data->url_directory)
		{
			this->_message += returnRedirectMessage();
			return (1);
		}
	}
	setStartLine();
	setHeaderField();
	this->_message += (this->_start_line + "\r\n");
	std::cout << "message out 1 : " << this->_message << std::endl;
	this->_message += this->_header_field;
	std::cout << "message out 2 : " << this->_message << std::endl;
	if (this->_data->isCGI == false &&
		this->_data->is_autoindex == false)
		this->_message += "\r\n";
	return (0);
}