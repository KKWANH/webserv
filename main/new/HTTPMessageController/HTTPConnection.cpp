#include "HTTPConnection.hpp"
#include <sstream>
extern NginxConfig::GlobalConfig _config;

HTTPConnection::HTTPConnection(int fd, int block, int server_port, std::string client_ip) {
	seq = REQUEST;
	socket_fd = fd;
	http_data = new HTTPData(block, server_port, client_ip);
	request_message = new RequestMessage(http_data);
	response_message = new ResponseMessage(http_data);
}

HTTPConnection::~HTTPConnection() {
	if (this->http_data->isCGI == true)
		delete cgi_process;
	delete request_message;
	delete response_message;
	delete http_data;
	close(socket_fd);
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
		if (request_result == RequestMessage::FINISH_PARSE) {
			if (this->http_data->isCGI == true) {
				cgi_process = new CGIProcess(http_data);
				cgi_process->run();
				cgi_input_fd = cgi_process->getInputPair();
				cgi_output_fd = cgi_process->getOutputPair();
				if (fcntl(cgi_input_fd, F_SETFL, O_NONBLOCK) == -1 ||
					fcntl(cgi_output_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"something wrong with fd. check the file exists : ?", ErrorHandler::NON_CRIT);
			}
			if (http_data->header_field.find("Content-Length") != http_data->header_field.end() &&
				(http_data->header_field["Content-Length"] != "0" && http_data->header_field["Content-Length"] != ""))
				seq = READY_TO_MESSAGE_BODY;
			else
				seq = REQUEST_TO_RESPONSE;
		}
		else if (request_result == RequestMessage::MESSAGE_BODY) {
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
		readLength = read(socket_fd, buffer, BUF_SIZ-1);
		if (readLength > 0) {
			buffer[readLength] = '\0';
			seq = MESSAGE_BODY_WRITE;
		}
		else {
			seq = REQUEST_TO_RESPONSE;
		}
	}
	else if (seq == MESSAGE_BODY_WRITE) {
		write(cgi_input_fd, buffer, readLength);
		seq = MESSAGE_BODY_READ;
	}
	else if (seq == REQUEST_TO_RESPONSE) {
		response_message->setResponseMessage(http_data->_tmp_directory);
		seq = RESPONSE;
	}
	else if (seq == RESPONSE) {
		int
			write_size = ((int)response_message->getMessage().size() < BUF_SIZ ? (int)response_message->getMessage().size() : BUF_SIZ);
		writeLength = write(socket_fd, response_message->getMessage().data(), write_size);
		if (writeLength != BUF_SIZ) {
			if (this->http_data->isCGI == true) {
				seq = READY_TO_CGI;
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
		readLength = read(cgi_output_fd, buffer, BUF_SIZ);
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
			}
			if (writeLength != BUF_SIZ) {
				close(cgi_output_fd);
				seq = CLOSE;
			}
			else
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
		}
	}
	return seq;
};