#ifndef REQUESTMSGCONTROLLER_HPP
# define REQUESTMSGCONTROLLER_HPP

#include "./httpMsgController.hpp"

// HTTP 요청 메세지 형태 ex)
// GET / HTTP/1.1
// Host: developer.mozilla.org
// Accept-Language: fr

// response 의 변수는은 protected 이지만, request 변수는 왜 public 인가...?
class requestMsg : public HTTPMsg {
	public:
		std::string	method;
		std::string	request_target;
		std::string	isHTTP;
		double		http_version;

		int			checkMethod(void) {
			if (method.compare("GET") == 0 ||
					method.compare("POST") == 0 ||
					method.compare("PUT") == 0 ||
					method.compare("PATCH") == 0 ||
					method.compare("DELETE") == 0 ||
					method.compare("HEAD") == 0 ||
					method.compare("CONNECT") == 0 ||
					method.compare("OPTIONS") == 0 ||
					method.compare("TRACE") == 0)
				return (0);
			return (-1);
		}

		// uri가 올바른지 판단하는 함수
		int			checkTarget(void) {
			if (request_target.length() > 0 && request_target[0] == '/')
				return (0);
			return (-1);
		}

		int			checkHTTP(void) {
			if (isHTTP.compare("HTTP/") == 0)
				return (0);
			return (-1);
		}

		int			checkHttpVersion(void) {
			if (http_version == 1.0 || http_version == 1.1 || http_version == 2.0)
				return (0);
			return (-1);
		}

		int			parseStartLine(std::string &msg) {
			int	start = 0;
			int	last = msg.find(' ');

			method = msg.substr(start, last);
			if (checkMethod() == -1)
				return (-1);

			start = last + 1;
			last = msg.find(' ' , start);
			request_target = msg.substr(start, last - start);
			if (checkTarget() == -1)
				return (-1);

			start = last + 1;
			http_version = atof(msg.substr(start).c_str());
			if (checkHttpVersion() == -1)
				return (-1);

			// header_field parsing을 위해 start_line 이후의 위치를 반환
			return (start + 5);
		}

		void		printStartLine(void) {
			std::cout << "Method\t: " << method << std::endl;
			std::cout << "Target\t: " << request_target << std::endl;
			std::cout << "isHTTP\t: " << isHTTP << std::endl;
			std::cout << "Version\t: " << http_version << std::endl;
		}

		static int	parsingRequestMsg(int client_sock, requestMsg *requestMsg) {
			int		n;
			char	charMsg[1024];
			read(client_sock, &charMsg, 1024);
			std::string stringMsg(charMsg);

			if ((n = requestMsg->parseStartLine(stringMsg)) == -1) {
				std::cout << "Error: parsing start line" << std::endl;
				close(client_sock);
				return (-1);
			}
			requestMsg->parseHeaderField(stringMsg, n);
			return (0);
		}

		static void		printRequestMsg(requestMsg *requestMsg, struct sockaddr_in client_addr) {
			requestMsg->printStartLine();
			requestMsg->printHeaderField();
			std::cout << "CLI ADDR\t: " << inet_ntoa(client_addr.sin_addr) << std::endl;
			std::cout << "CLI PORT\t: " << ntohs(client_addr.sin_port) << std::endl;
		}
};

#endif
