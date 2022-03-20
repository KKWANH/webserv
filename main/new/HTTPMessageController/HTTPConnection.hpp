#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "ClassController.hpp"
#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"
#include "SocketController.hpp"
#include "KernelQueueController.hpp"

#include <fcntl.h>
#include "FileController.hpp"

#define BUF_SIZ 8192

extern NginxConfig::GlobalConfig _config;

class HTTPConnection : public ClassController {
	public:
		typedef enum	e_Seq {
			REQUEST,
			REQUEST_TO_RESPONSE,
			RESPONSE,
			READY_TO_FILE,
			FILE_READ,
			FILE_WRITE,
			CLOSE,
			END
		}				Seq;

	private:
		Seq						seq;
		int						socket_fd;
		int						file_fd;
		char					file_buffer[BUF_SIZ];
		HTTPData*				http_data;
		RequestMessage*			request_message;
		ResponseMessage*		response_message;
		int						readLength;
		int						writeLength;
		
	public:
		HTTPConnection(int fd, int block, SocketController *Socket) {
			seq = REQUEST;
			socket_fd = fd;
			http_data = new HTTPData(block, Socket);
			request_message = new RequestMessage(http_data);
			response_message = new ResponseMessage(http_data);
		}
		virtual ~HTTPConnection() {
			delete request_message;
			delete response_message;
			delete http_data;
			close(socket_fd);
		}

		int	getFileFd() {
			return (this->file_fd);
		}

		int	getSocketFd() {
			return (this->socket_fd);
		}
		
		int run() {
			if (seq == REQUEST) {
				char buffer[BUF_SIZ];
				readLength = read(socket_fd, buffer, BUF_SIZ);
				if (readLength > 0)
					request_message->setMessage(buffer);
				if (request_message->parsingRequestMessage() == RequestMessage::FINISH_PARSE)
					seq = REQUEST_TO_RESPONSE;
			}
			else if (seq == REQUEST_TO_RESPONSE) {
				response_message->setResponseMessage(request_message->getTmpDirectory());
				seq = RESPONSE;
			}
			else if (seq == RESPONSE) {
				int	write_size = ((int)response_message->getMessage().size() < BUF_SIZ ? (int)response_message->getMessage().size() : BUF_SIZ);
				writeLength = write(socket_fd, response_message->getMessage().data(), write_size);
				if (writeLength != BUF_SIZ) {
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
				else
					response_message->resetMessage(writeLength);
			}
			else if (seq == READY_TO_FILE) {
				seq = FILE_READ;
			}
			else if (seq == FILE_READ) {
				readLength = read(file_fd, file_buffer, 2048);
				seq = FILE_WRITE;
			}
			else if (seq == FILE_WRITE) {
				if (readLength == 0)
					seq = CLOSE;
				else if (readLength == -1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,
						"exit -1 replaced", ErrorHandler::CRIT);

				else {
					writeLength = write(socket_fd, file_buffer, readLength);
					if (readLength != writeLength) {
						throw ErrorHandler(__FILE__, __func__, __LINE__,
							"exit -1 replaced", ErrorHandler::CRIT);
					}
					if (writeLength != 2048) {
						close(file_fd);
						seq = CLOSE;
					}
					else
						seq = FILE_READ;
				}
			}
			return seq; // 상호참조를 줄이기 위해 저는 클래스의 외부에서 커널큐 상태를 변경하는 것을 선호합니다.
		}
};

#endif