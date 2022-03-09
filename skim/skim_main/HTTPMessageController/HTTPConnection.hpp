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
			READ,
			REQUEST,
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
		//int cgi_fd; // not used

	public:
		HTTPConnection(int fd, int block) {
			seq = READ;
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
			if (seq == READ) {
				char buffer[1024];
				readLength = read(socket_fd, buffer, 1024);
				if (readLength > 0)
					request_message->setMessage(buffer);
				if (request_message->parsingRequestMessage() == RequestMessage::FINISH_PARSE)
					seq = RESPONSE;
			} else if (seq == RESPONSE) {
				// std::string httpTestHeaderString;
				// httpTestHeaderString += "HTTP/1.1 200 OK\r\n";
				// httpTestHeaderString += "Content-Type: text/html\r\n";
				// httpTestHeaderString += "\r\n";
				// httpTestHeaderString += "<html>";
				// httpTestHeaderString += "<head><title>hi</title></head>";
				// httpTestHeaderString += "<body>";
				// httpTestHeaderString += "<b><center> HI! </center></b>";
				// httpTestHeaderString += "</body>";
				// httpTestHeaderString += "</html>";
				// write(socket_fd, httpTestHeaderString.data(), httpTestHeaderString.length());
				// Response 시퀀스가 완료되면 다음 시퀀스로 넘어가게 해야 합니다...
				response_message->setResponseMessage();
				write(socket_fd, response_message->getResponseMessage().data(), response_message->getResponseMessage().length());
				seq = CLOSE;
			} else if (seq == CLOSE) {
				seq = END;
			}
			return seq; // 상호참조를 줄이기 위해 저는 클래스의 외부에서 커널큐 상태를 변경하는 것을 선호합니다.
		}
};

#endif
