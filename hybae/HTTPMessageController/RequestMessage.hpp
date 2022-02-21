#include "./HTTPMessageController.hpp"

// Request Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class RequestMessage : public HTTPMessage {
	private:
		std::string		method;
		std::string		request_target;
		std::string		uri_dir;
		std::string		uri_file;
		std::string     query_string;
		std::string		isHTTP;
		double			http_version;

	public:

        void    printVar() {
            std::cout << __func__ << std::endl;
            std::cout << "method : " << method << std::endl;
            std::cout << "request_target : " << request_target << std::endl;
            std::cout << "uri_dir : " << uri_dir << std::endl;
            std::cout << "uri_file : " << uri_file << std::endl;
			std::cout << "query_string : " << query_string << std::endl;
            std::cout << "isHTTP : " << isHTTP << std::endl;
            std::cout << "http_version : " << http_version << std::endl;
            std::cout << "----------------------" << std::endl;
        }

		std::string		getMethod()			{ return method; }
		std::string		getRequestTarget()	{ return request_target; }
		std::string		getUriDir()			{ return uri_dir; }
		std::string		getUriFile()		{ return uri_file; }
		double			getHttpVersion()	{ return http_version; }

		// reset R equest message
		void					resetMessage() {
			start_line = "";
			header_field.clear();
			message_body = "";
			method = "";
			request_target = "";
			uri_dir = "";
			uri_file = "";
			query_string = "";
			isHTTP = "";
			http_version = 0;
			return ;
		}

		// HTTP Method parsing 및 에러 처리
		int						parseMethod(int* start, int* end, std::string& message) {
			*start = 0;
			*end = message.find(' ');
			if (*end == -1)
				return (-1);
			this->method = message.substr(*start, *end);;
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

		// HTTP uri parsing 및 에러 처리
		int						parseTarget(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find(' ', *start);
			if (*end == -1)
				return (-1);
			this->request_target = message.substr(*start, *end - *start);
			if (request_target.length() > 0 && request_target[0] == '/') {
				int dir_pos = request_target.find_last_of("/");
				int file_pos = request_target.find_last_of(".");
				size_t	query_pos = request_target.find_last_of("?");
				if (dir_pos < file_pos) {
					this->uri_dir = request_target.substr(0, dir_pos);
					if (query_pos == std::string::npos)
						this->uri_file = request_target.substr(dir_pos + 1);
					else {
						this->uri_file = request_target.substr(dir_pos + 1, query_pos - dir_pos - 1);
						this->query_string = request_target.substr(query_pos + 1);
					}
				}
				else {
					this->uri_dir = request_target;
					this->uri_file = config.getConfig("index");
				}
				return (0);
			}
			return (-1);
		}

		// HTTP protocol check 및 에러 처리
		int						parseIsHTTP(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find('/', *start);
			if (*end == -1)
				return (-1);
			this->isHTTP = message.substr(*start, *end - *start + 1);

			if (isHTTP.compare("HTTP/") == 0)
				return (0);
			return (-1);
		}

		// HTTP version check 및 에러 처리
		int						parseHTTPVersion(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find("\r\n", *start);
			if (*end == -1)
				return (-1);
			http_version = atof(message.substr(*start).c_str());

			if (http_version == 1.0 ||
					http_version == 1.1 ||
					http_version == 2.0)
				return (0);
			return (-1);
		}

		// start_line을 parsing하여 각 변수에 담는 함수
		// start_line의 데이터는 필수이므로 각 데이터에 대한 예외 처리
		// header_field parsing을 위해 start_line 이후 문자의 인덱스 반환
		int						parseStartLine(std::string& message) {
			int	start, end;

			if (this->parseMethod(&start, &end, message) == -1)
				return (-1);
			if (this->parseTarget(&start, &end, message) == -1)
				return (-1);
			if (this->parseIsHTTP(&start, &end, message) == -1)
				return (-1);
			if (this->parseHTTPVersion(&start, &end, message) == -1)
				return (-1);
			return (start + 5);
		}

		// TODO: POST의 경우 message body도 parsing 필요함
		// Request Message parsing
		// start line, header field
		int						parsingRequestMessage(int fd, std::string& message) {
			int n;

			if ((n = this->parseStartLine(message)) == -1) {
				close(fd);
				throw ErrorHandler(__FILE__, __func__, __LINE__, "PARSE ERROR");
			}
			this->parseHeaderField(message, n);
            printVar();
			return (0);
		}

		// Test print
		void					printRequestMessage() {
			std::string rtn;

			std::cout << "METHOD : " << this->method << std::endl;
			std::cout << "DIR : " << this->uri_dir << std::endl;
			std::cout << "FILE : " << this->uri_file << std::endl;
			std::cout << "HTTP : " << this->isHTTP + std::to_string(this->http_version) << std::endl;

			return ;
		}
};