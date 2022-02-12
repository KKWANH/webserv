#ifndef REQUESTMSGCONTROLLER_HPP
# define REQUESTMSGCONTROLLER_HPP

#include "./configController.hpp"
#include "./MIMEController.hpp"
#include "./httpMsgController.hpp"

// HTTP 요청 메세지 형태 ex)
// GET / HTTP/1.1
// Host: developer.mozilla.org
// Accept-Language: fr

extern configController	config;
extern MIMEController	mime;

class requestMsg : public HTTPMsg {
	private:
		std::string	method;
		std::string	request_target;
		std::string	uri_dir;
		std::string	uri_file;
		std::string	isHTTP;
		double		http_version;

	public:
		std::string	getMethod(void) { return (method); }
		std::string	getRequestTarget(void) { return (request_target); }
		std::string	getUriDir(void) { return (uri_dir); }
		std::string getUriFile(void) { return (uri_file); }
		double		getHttpVersion(void) { return (http_version); }

		void		resetMessage(void) {
			start_line = "";
			header_field.clear();
			msg_body = "";
			method = "";
			request_target = "";
			uri_dir = "";
			uri_file = "";
			isHTTP = "";
			http_version = 0;
		}


		int			parseMethod(int *start, int *end, std::string &msg) {
			*start = 0;
			*end = msg.find(' ');
			if (*end == -1)
				return (-1);
			std::cout << msg.substr(*start, *end) << std::endl;
			method = msg.substr(*start, *end);

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
		int			parseTarget(int *start, int *end, std::string &msg) {
			*start = *end + 1;
			*end = msg.find(' ', *start);
			if (*end == -1)
				return (-1);
			this->request_target = msg.substr(*start, *end - *start);
			if (request_target.length() > 0 && request_target[0] == '/') {
				int dir_pos = request_target.find_last_of('/');
				int file_pos = request_target.find_last_of('.');
				if (dir_pos < file_pos) {
					uri_dir = request_target.substr(0, dir_pos);
					uri_file = request_target.substr(dir_pos + 1);
				} else {
					uri_dir = request_target;
					uri_file = config.getConfig("index");
				}
				return (0);
			}
			return (-1);
		}

		int			parseIsHTTP(int *start, int *end, std::string &msg) {
			*start = *end + 1;
			*end = msg.find('/', *start);
			if (*end == -1)
				return (-1);
			this->isHTTP = msg.substr(*start, *end - *start + 1);

			if (isHTTP.compare("HTTP/") == 0)
				return (0);
			return (-1);
		}

		int			parseHTTPVersion(int *start, int *end, std::string &msg) {
			*start = *end + 1;
			*end = msg.find("\r\n", *start);
			if (*end == -1)
				return (-1);
			http_version = atof(msg.substr(*start).c_str());

			if (http_version == 1.0 || http_version == 1.1 || http_version == 2.0)
				return (0);
			return (-1);
		}

		int			parseStartLine(std::string &msg) {
			int start, end;

			if (this->parseMethod(&start, &end, msg) == -1)
				return (-1);
			if (this->parseTarget(&start, &end, msg) == -1)
				return (-1);
			if (this->parseIsHTTP(&start, &end, msg) == -1)
				return (-1);
			if (this->parseHTTPVersion(&start, &end, msg) == -1)
				return (-1);
			return (start + 5);
		}

		int			parsingRequestMsg(int client_sock, std::string &msg) {
			int		n;

			std::cout << "client_sodk : " << client_sock << std::endl;
			std::cout << "msg : " << msg << std::endl;

			if ((n = this->parseStartLine(msg)) == -1) {
				std::cout << "Error: parsing start line" << std::endl;
				close(client_sock);
				return (-1);
			}
			this->parseHeaderField(msg, n);
			return (0);
		}

		void		printRequestMsg() {
			std::string rtn;

			std::cout << "METHOD : " << this->method << std::endl;
			std::cout << "DIR : " << this->uri_dir << std::endl;
			std::cout << "FILE : " << this->uri_file << std::endl;
			std::cout << "HTTP : " << this->isHTTP + std::to_string(this->http_version) << std::endl;

			return ;
		}
};

#endif
