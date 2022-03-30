#include "ResponseMessage.hpp"

ResponseMessage::ResponseMessage(HTTPData* _tmp) {
	this->_data = _tmp;
	this->_message = "";
	this->_start_line = "";
	this->_header_field = "";
	this->_status_msg_path = "./setting/status_code.txt";
}

std::string
	ResponseMessage::returnRedirectMessage() {
	std::string
		redirectMessage = "HTTP/1.1 308 Permanent Redirect\r\nLocation: ";
	int location_code = 6;
	for (int i=0; i<(int)_config._http._server[_data->server_block]._location.size(); i++)
	{
		if (this->_data->uri_dir == _config._http._server[_data->server_block]._location[i]._location)
		{
			location_code = i;
			break;
		}
	}
	redirectMessage += _config._http._server[_data->server_block]._location[location_code]._rewrite[1];
	redirectMessage += "\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: 0\r\n\r\n";
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

	temp << this->_data->status_code;
	temp >> str_status_code;

	this->_start_line += "HTTP/1.1 ";
	this->_start_line += str_status_code;
	this->_start_line += " ";
	this->_start_line += setStatusMessage(str_status_code);
	return ;
}

void	ResponseMessage::setHeaderField() {
	std::string
		_path = _config._http._server[this->_data->server_block]._dir_map["root"] + this->_data->uri_dir + this->_data->uri_file;
	if (this->_data->isCGI == true) {
		// FIXME
		// CGI의 경우, Chunked _data를 받아야 함.
		//this->_header_field += "Transfer-Encoding: chunked\r\n";
		//this->_header_field += "Content-Length: 51\r\n";
	}
	else if (this->_data->is_autoindex == true ||
			 (this->_data->status_code >= 400 && this->_data->status_code <= 600))
	{
		this->_header_field += ("Content-Type: " + _mime.getMIME(this->_data->file_extension) + "\r\n");

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
	for (int _idx=0; _idx<(int)(_config._http._server[this->_data->server_block]._location.size()); _idx++)
	{
		if (_config._http._server[this->_data->server_block]._location[_idx]._location == this->_data->url_directory &&
			(int)_config._http._server[this->_data->server_block]._location[_idx]._rewrite.size() != 0 &&
			_config._http._server[this->_data->server_block]._location[_idx]._rewrite[0] == this->_data->url_directory)
		{
			this->_message += returnRedirectMessage();
			return (1);
		}
	}
	setStartLine();
	setHeaderField();
	this->_message += (this->_start_line + "\r\n");
	this->_message += this->_header_field;
	if (this->_data->isCGI == false &&
		this->_data->is_autoindex == false &&
		(this->_data->status_code < 400 || this->_data->status_code >= 600) &&
		this->_data->is_buffer_write == false)
		this->_message += "\r\n";
	return (0);
}