#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "ClassController.hpp"
#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"

extern NginxConfig::GlobalConfig _config;

class HTTPConnection : public ClassController {
	public:
		typedef enum	e_Seq {
			REQUEST,
			REQUEST_TO_RESPONSE,
			RESPONSE,
			CLOSE,
			END
		}				Seq;

	private:
		Seq					seq;
		int					socket_fd;
		HTTPData*			http_data;
		RequestMessage*		request_message;
		ResponseMessage*	response_message;
		int					readLength;
		int					writeLength;
		//int cgi_fd; // not used
		
	public:
		HTTPConnection(int fd, int block) {
			seq = REQUEST;
			socket_fd = fd;
			http_data = new HTTPData(block);
			request_message = new RequestMessage(http_data);
			response_message = new ResponseMessage(http_data);
		}
		virtual ~HTTPConnection() {
			close(socket_fd);
			delete request_message;
			delete response_message;
			delete http_data;
		}

		int run() {
			std::cout << __FILE__ << " : " << __func__ << " : " << seq << std::endl;
			if (seq == REQUEST) {
				char buffer[4096];
				readLength = read(socket_fd, buffer, 4096);
				if (readLength > 0)
					request_message->setMessage(buffer);
				if (request_message->parsingRequestMessage() == RequestMessage::FINISH_PARSE)
					seq = REQUEST_TO_RESPONSE;
			}
			else if (seq == REQUEST_TO_RESPONSE) {
				std::cout << "REQUEST_TO_RESPONSE" << std::endl;
				response_message->setResponseMessage();
				seq = RESPONSE;
			}
			else if (seq == RESPONSE) {
				std::cout << "RESPONSE" << std::endl;
				int	write_size = ((int)response_message->getMessage().size() < 8192 ? (int)response_message->getMessage().size() : 8192);
				writeLength = write(socket_fd, response_message->getMessage().data(), write_size);
				if (writeLength != 8192)
					seq = CLOSE;
				else
					response_message->resetMessage(writeLength);
			}
			else if (seq == CLOSE) {
				seq = END;
			}
			return seq; // 상호참조를 줄이기 위해 저는 클래스의 외부에서 커널큐 상태를 변경하는 것을 선호합니다.
		}
};

#endif
