#include "HTTPConnection.hpp"

extern NginxConfig::GlobalConfig _config;

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
	request_message = new RequestMessage(http_data);
	response_message = new ResponseMessage(http_data);
	if (_config._http._server[
			this->http_data->server_block]._dir_map[
				"client_max_body_size"].empty() == true)
		limit_size = 1024;
	else
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
	std::cout << "sequence : " << seq << std::endl;
	if (seq == REQUEST) {
		std::cout << "[REQUEST]\n";
		readLength = read(socket_fd, buffer, BUF_SIZ-1);
		buffer[readLength] = '\0';
		if (readLength > 0)
			request_message->setMessage(buffer);
		int request_result = request_message->parsingRequestMessage();
		if (request_result == RequestMessage::ERROR)
		{
			std::string
				_msg_body = ErrorPageController::getErrorBody(request_message->error_code);
			http_data->str_buffer = "Content-Length: ";
			std::stringstream ss;
			ss << _msg_body.size();
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
			if (http_data->header_field["Content-Length"].empty() == false &&
				atoi(http_data->header_field["Content-Length"].c_str()) > limit_size)
			{
				std::string
					_msg_body = ErrorPageController::getErrorBody("413");
				http_data->str_buffer = "Content-Length: ";
				std::stringstream ss;
				ss << _msg_body.size();
				http_data->str_buffer += ss.str();
				http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
				http_data->str_buffer += _msg_body;
				seq = REQUEST_TO_RESPONSE;
			}
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
			if (http_data->header_field["Content-Length"].empty() == false &&
				atoi(http_data->header_field["Content-Length"].c_str()) > limit_size)
			{
				std::string
					_msg_body = ErrorPageController::getErrorBody("413");
				http_data->str_buffer = "Content-Length: ";
				std::stringstream ss;
				ss << _msg_body.size();
				http_data->str_buffer += ss.str();
				http_data->str_buffer += "\r\nContent-Type: text/html\r\n\r\n";
				http_data->str_buffer += _msg_body;
				http_data->is_buffer_write = true;
				seq = REQUEST_TO_RESPONSE;
			}
		}
	}
	else if (seq == READY_TO_MESSAGE_BODY) {
		std::cout << "[READY_TO_MESSAGE_BODY]\n";
		write(cgi_input_fd, request_message->getMessage().data(), request_message->getMessage().size());
		std::stringstream ss;
		ss << request_message->getMessage().size();
		if (ss.str() == http_data->header_field["Content-Length"])
			seq = BODY_TO_RESPONSE;
		else
			seq = MESSAGE_BODY_READ;
	}
	else if (seq == BODY_TO_RESPONSE) {
		std::cout << "[BODY_TO_RESPONSE]\n";
		seq = REQUEST_TO_RESPONSE;
	}
	else if (seq == MESSAGE_BODY_READ) {
		std::cout << "[MESSAGE_BODY_READ]" << std::endl;
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
		std::cout << "[MESSAGE_BODY_WRITE]" << std::endl;
		writeLength = write(cgi_input_fd, buffer, readLength);
		if (writeLength == BUF_SIZ - 1)
			seq = MESSAGE_BODY_READ;
		else
			seq = REQUEST_TO_RESPONSE;
	}
	else if (seq == REQUEST_TO_RESPONSE) {
		std::cout << "[REQUEST_TO_RESPONSE]" << std::endl;
		if (http_data->is_buffer_write == false)
			response_message->setResponseMessage();
		seq = RESPONSE;
	}
	
};
