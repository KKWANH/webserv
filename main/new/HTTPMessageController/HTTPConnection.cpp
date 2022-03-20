#include "HTTPConnection.hpp"

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
		
int HTTPConnection::run() {
	if (seq == REQUEST) {
		readLength = read(socket_fd, buffer, BUF_SIZ);
		if (readLength > 0)
			request_message->setMessage(buffer);
		if (request_message->parsingRequestMessage() == RequestMessage::FINISH_PARSE)
			seq = this->http_data->isCGI ? IS_CGI : REQUEST_TO_RESPONSE;
	}
	else if (seq == REQUEST_TO_RESPONSE) {
		response_message->setResponseMessage(request_message->getTmpDirectory());
		seq = RESPONSE;
	}
	else if (seq == RESPONSE) {
		int	write_size = ((int)response_message->getMessage().size() < BUF_SIZ ? (int)response_message->getMessage().size() : BUF_SIZ);
		writeLength = write(socket_fd, response_message->getMessage().data(), write_size);
		if (writeLength != BUF_SIZ) {
			if (this->http_data->isCGI == true) {
				seq = CGI_WRITE;
			}
			else {
				std::string	path =
						_config._http._server[this->http_data->server_block]._dir_map["root"] +
						this->http_data->uri_dir +
						this->http_data->uri_file;
				file_fd = open(path.c_str(), O_RDONLY);
				if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"fd Changed. Maybe redirected by RequestMessage income. :) ", ErrorHandler::NON_CRIT);
				seq = READY_TO_FILE;
			}
		}
		else
			response_message->resetMessage(writeLength);
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
	// FIXME
	// Chunked data에 대해 Body를 인식하지 못 함
	else if (seq == CGI_WRITE) {
		int cgi_read_fd = cgi_process->getOutputPair();
		readLength = 1;
		while (readLength != 0) {
			readLength = read(cgi_read_fd, buffer, BUF_SIZ);
			writeLength = write(socket_fd, buffer, readLength);
			std::cout << "----------------CGI-WRITE------------------" << std::endl;
			std::cout << std::string(buffer).substr(0, writeLength) << std::endl;
			std::cout << "-------------------------------------------" << std::endl;
		}
		if (writeLength != BUF_SIZ) {
			std::cout << "FINISH CGI WRITE" << std::endl;
			seq = CLOSE;
		}
	}
	if (seq == IS_CGI) {
		cgi_process = new CGIProcess(http_data);
		cgi_process->run();
		this->seq = REQUEST_TO_RESPONSE;
	}
	return seq;
};