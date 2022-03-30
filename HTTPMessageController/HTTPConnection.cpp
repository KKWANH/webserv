#include "HTTPConnection.hpp"

extern NginxConfig::GlobalConfig
	_config;

HTTPConnection::HTTPConnection(
	int fd,
	int block,
	int server_port,
	std::string client_ip,
	std::string client_port,
	std::string host_ip,
	std::string host_port)
{
	seq = REQUEST;
	socket_fd = fd;
	http_data = new HTTPData(block, server_port, client_ip);
	http_data->client_ip = client_ip;
	http_data->client_port = client_port;
	http_data->host_ip = host_ip;
	http_data->host_port = host_port;
	error_page_controller = new ErrorPageController();
	error_page_controller->setHTTPData(http_data);
	autoindex = new AutoindexController(http_data);
	request_message = new RequestMessage(http_data);
	response_message = new ResponseMessage(http_data);
	limit_size = 1048576;
	// if (_config._http._server[
	// 		this->http_data->server_block]._dir_map[
	// 			"client_max_body_size"].empty() == true)
	// 	limit_size = 1024 * 1024;
	if (_config._http._server[
			this->http_data->server_block]._dir_map[
				"client_max_body_size"].empty() == false)
		limit_size =
			atoi(
				_config._http._server[
					this->http_data->server_block]._dir_map[
						"client_max_body_size"].c_str());
	current_size = 0;
}

HTTPConnection::~HTTPConnection() {
	if (this->http_data->isCGI == true)
		delete cgi_process;
	delete request_message;
	delete response_message;
	delete http_data;
	delete error_page_controller;
	delete autoindex;
	close(socket_fd);
}

void HTTPConnection::killConnection(void* hc) {
	delete reinterpret_cast<HTTPConnection*>(hc);
}

int	HTTPConnection::getServerBlock(void)	{ return (this->http_data->getSBlock()); }
int	HTTPConnection::getFileFd(void)			{ return (this->file_fd); }
int	HTTPConnection::getSocketFd(void)		{ return (this->socket_fd); }
int	HTTPConnection::getCgiOutputFd(void)	{ return (this->cgi_output_fd); }
int	HTTPConnection::getCgiInputFd(void)		{ return (this->cgi_input_fd); }

int HTTPConnection::run() {
	if (seq == REQUEST) {
		readLength = read(socket_fd, buffer, BUF_SIZ-1);
		buffer[readLength] = '\0';
		if (readLength > 0)
			request_message->setMessage(buffer);
		int request_result = request_message->parsingRequestMessage();
		if (request_result == RequestMessage::ERROR
			)
		{
			std::string
				_msg_body = error_page_controller->findErrorPage(std::to_string(http_data->status_code));
			std::stringstream ss;
			ss << _msg_body.size();
			http_data->str_buffer = "Content-Length: ";
			http_data->str_buffer += ss.str();
			http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
			http_data->str_buffer += _msg_body;
			http_data->is_buffer_write = true;
			seq = REQUEST_TO_RESPONSE;
		}
		else if (request_result == RequestMessage::FINISH_PARSE) {
			std::map<std::string, std::string>::iterator res;
			if ((res = this->http_data->header_field.find("Connection")) != this->http_data->header_field.end())
				if (res->second == "keep-alive")
					keep_alive = true;
			if (http_data->header_field.find("Content-Length") != http_data->header_field.end() &&
				(http_data->header_field["Content-Length"] != "0" && http_data->header_field["Content-Length"] != ""))
				seq = READY_TO_MESSAGE_BODY;
			else
				seq = REQUEST_TO_RESPONSE;
			if (http_data->header_field["Content-Length"].empty() == false &&
				atoi(http_data->header_field["Content-Length"].c_str()) > limit_size)
			{
				std::string
					_msg_body = error_page_controller->findErrorPage("413");
				http_data->str_buffer = "Content-Length: ";
				http_data->status_code = 413;
				http_data->isCGI = false;
				std::stringstream ss;
				ss << _msg_body.length();
				http_data->str_buffer += ss.str();
				http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
				http_data->str_buffer += _msg_body;
				http_data->is_buffer_write = true;
				seq = REQUEST_TO_RESPONSE;
			}
			if (this->http_data->isCGI == true) {
				cgi_process = new CGIProcess(http_data);
				cgi_process->run();
				cgi_input_fd = cgi_process->getInputPair();
				cgi_output_fd = cgi_process->getOutputPair();
				if (fcntl(cgi_input_fd, F_SETFL, O_NONBLOCK) == -1 ||
					fcntl(cgi_output_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"something wong with fd. check the file exists : ?", ErrorHandler::NON_CRIT);
			}
		}
		else if (request_result == RequestMessage::MESSAGE_BODY) {
			/*
			if (this->http_data->isCGI == true) {
				cgi_process = new CGIProcess(http_data);
				cgi_process->run();
				cgi_input_fd = cgi_process->getInputPair();
				cgi_output_fd = cgi_process->getOutputPair();
				if (fcntl(cgi_input_fd, F_SETFL, O_NONBLOCK) == -1 ||
					fcntl(cgi_output_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"something wrong with fd. check the file exists : ?", ErrorHandler::NON_CRIT);
				seq = READY_TO_MESSAGE_BODY;
			}
			else
				seq = REQUEST_TO_RESPONSE;
				*/
			if (http_data->header_field["Content-Length"].empty() == false &&
				atoi(http_data->header_field["Content-Length"].c_str()) > limit_size)
			{
				std::string
					_msg_body = error_page_controller->findErrorPage("413");
				http_data->str_buffer = "Content-Length: ";
				http_data->status_code = 413;
				http_data->isCGI = false;
				std::stringstream ss;
				ss << _msg_body.length();
				http_data->str_buffer += ss.str();
				http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
				http_data->str_buffer += _msg_body;
				http_data->is_buffer_write = true;
				seq = REQUEST_TO_RESPONSE;
			}
			if (this->http_data->isCGI == true) {
				cgi_process = new CGIProcess(http_data);
				cgi_process->run();
				cgi_input_fd = cgi_process->getInputPair();
				cgi_output_fd = cgi_process->getOutputPair();
				if (fcntl(cgi_input_fd, F_SETFL, O_NONBLOCK) == -1 ||
					fcntl(cgi_output_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"something wrong with fd. check the file exists : ?", ErrorHandler::NON_CRIT);
				seq = READY_TO_MESSAGE_BODY;
			}
			else
				seq = REQUEST_TO_RESPONSE;
		}
	}
	else if (seq == READY_TO_MESSAGE_BODY) {
		write(cgi_input_fd, request_message->getMessage().data(), request_message->getMessage().size());
		std::stringstream ss;
		ss << request_message->getMessage().size();
		if (ss.str() == http_data->header_field["Content-Length"])
			seq = BODY_TO_RESPONSE;
		else
			seq = MESSAGE_BODY_READ;
	}
	else if (seq == BODY_TO_RESPONSE) {
		seq = REQUEST_TO_RESPONSE;
	}
	else if (seq == MESSAGE_BODY_READ) {
		readLength = read(socket_fd, buffer, BUF_SIZ - 1);
		buffer[readLength] = '\0';
		if (readLength > 0) {
//			buffer[readLength] = '\0';
			seq = MESSAGE_BODY_WRITE;
		}
		else {
			seq = REQUEST_TO_RESPONSE;
		}
	}
	else if (seq == MESSAGE_BODY_WRITE) {
		writeLength = write(cgi_input_fd, buffer, readLength);
		if (writeLength == BUF_SIZ - 1)
			seq = MESSAGE_BODY_READ;
		else
			seq = REQUEST_TO_RESPONSE;
	}
	else if (seq == REQUEST_TO_RESPONSE) {
		response_message->setResponseMessage();
		response_message->printStartLine();
		response_message->printHeaderField();
		seq = RESPONSE;
	}
	else if (seq == RESPONSE) {
		int
			write_size = ((int)response_message->getMessage().size() < BUF_SIZ ? (int)response_message->getMessage().size() : BUF_SIZ);
		writeLength = write(socket_fd, response_message->getMessage().data(), write_size);
		if (http_data->is_buffer_write == true)
		{
			write(socket_fd, http_data->str_buffer.data(), http_data->str_buffer.size());
			http_data->is_buffer_write = false;
			seq = CLOSE;
		}
		else if (writeLength != BUF_SIZ) {
			if (this->http_data->isCGI == true) {
				seq = READY_TO_CGI;
			}
			else if (this->http_data->is_autoindex == true) {
				std::string
					_msg_body = autoindex->getAutoIndexBody(_config._http._server[http_data->server_block]._dir_map["root"], this->http_data->url_directory);
				http_data->str_buffer = "Content-Length: ";
				std::stringstream ss;
				ss << _msg_body.size();
				http_data->str_buffer += ss.str();
				http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
				http_data->str_buffer += _msg_body;
				http_data->is_buffer_write = true;
				seq = REQUEST_TO_RESPONSE;
			}
			else {
				std::string	_pth =
					_config._http._server[this->http_data->server_block]._dir_map["root"] +
					this->http_data->uri_dir +
					this->http_data->uri_file;
				_pth = FileController::toAbsPath(_pth);
				file_fd = open(_pth.c_str(), O_RDONLY);
				if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"something wrong with fd. check the file exists : \n" + _pth, ErrorHandler::NON_CRIT);
				seq = READY_TO_FILE;
 			}
		}
		else
			response_message->resetMessage(writeLength);
	}
	else if (seq == READY_TO_CGI) {
		close(cgi_input_fd);
		seq = CGI_READ;
	}
	else if (seq == CGI_READ) {
		readLength = read(cgi_output_fd, buffer, BUF_SIZ - 1);
		buffer[readLength] = '\0';
		seq = CGI_WRITE;
	}
	else if (seq == CGI_WRITE) {
		if (readLength == 0)
			seq = CLOSE;
		else if (readLength == -1)
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"exit -1 replaced", ErrorHandler::CRIT);
		else {
			writeLength = write(socket_fd, buffer, readLength);
			if (readLength != writeLength) {
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"exit -1 replaced", ErrorHandler::CRIT);
			}/*
			if (writeLength != BUF_SIZ - 1) {
				close(cgi_output_fd);
				seq = CLOSE;
			}
			else
			*/
			seq = CGI_READ;
		}
	}
	else if (seq == READY_TO_FILE) {
		seq = FILE_READ;
	}
	else if (seq == FILE_READ) {
		readLength = read(file_fd, buffer, BUF_SIZ);
		seq = FILE_WRITE;
	}
	else if (seq == FILE_WRITE) {
		if (readLength == 0)
			seq = CLOSE;
		else if (readLength == -1)
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"exit -1 replaced", ErrorHandler::CRIT);
		else {
			writeLength = write(socket_fd, buffer, readLength);
			if (readLength != writeLength) {
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"exit -1 replaced", ErrorHandler::CRIT);
			}
			if (writeLength != BUF_SIZ) {
				close(file_fd);
				seq = CLOSE;
			}
			else
				seq = FILE_READ;
			if (seq == CLOSE && keep_alive == true)
				seq = RE_KEEPALIVE;
		}
	}
	else if (seq == RE_KEEPALIVE) {
		int 		backup_block;
		int 		backup_port;
		std::string backup_ip;

		backup_block = this->http_data->server_block;
		backup_port = this->http_data->server_port;
		backup_ip = this->http_data->client_ip;

		if (this->http_data->isCGI == true)
			delete cgi_process;
		delete request_message;
		delete response_message;
		delete http_data;
		delete error_page_controller;
		delete autoindex;
		buffer[0] = '\0';
		readLength = -2;
		writeLength = -2;
		if (file_fd > 0)
			file_fd = -2;
		if (cgi_output_fd > 0)
			cgi_output_fd = -2;
		if (cgi_input_fd > 0)
			cgi_input_fd = -2;

		// keep_alive = false;
		//다음 연결에서 keep-alive가 안되는 경우도 있다?

		http_data = new HTTPData(backup_block, backup_port, backup_ip);
		request_message = new RequestMessage(http_data);
		response_message = new ResponseMessage(http_data);
		error_page_controller = new ErrorPageController();
		error_page_controller->setHTTPData(http_data);
		autoindex = new AutoindexController(http_data);
		seq = REQUEST;
	}
	return seq;
};
