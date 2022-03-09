#ifndef REQUESTMESSAGE_HPP
# define REQUESTMESSAGE_HPP

#include <iostream>
#include <unistd.h>
#include <string>
#include "HTTPData.hpp"
#include "ErrorHandler.hpp"
#include "CGIProcess.hpp"
#include "ConfigController.hpp"

extern ConfigController config;

class RequestMessage {
	private:
		HTTPData data;
		CGIProcess cgi;
	public:
		RequestMessage(HTTPData &_data, CGIProcess &_cgi) : data(_data), cgi(_cgi) {}

		void	parsingRequestMessage(int fd, std::string &msg) {

			// request msg - start_line
			int pos = this->parseStartLine(msg);
			if (pos== -1) { // pos가 -1이 될 경우가 있는가...?
				close(fd);
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Start line parsing error in Request msg");
			}
			printStartLine();

			// if (data->isCGI) {
			// 	cgi.setEnvp(this);
			// 	cgi.run();
			// }

			// request msg - header_field
			pos = this->parseHeaderField(msg, pos);
			printHeaderField();

			// request msg - body
			this->parseBody(msg, pos);
			printBody();

			if (this->data.isCGI) {
				// message body가 있다면
				if (this->data.header_field.find("Content-Length") != this->data.header_field.end() && this->data.header_field["Content-Length"] != "0") {
					std::cout << "CGI DATA : " << msg.c_str() << std::endl;
					write(cgi.getInputPair(), msg.c_str(), msg.length());
				}
				char buf[1024];
				read(cgi.getOutputPair(), buf, 1024);
				std::cout << "buf : " << buf << std::endl;
			}
		}

		/** Header **/
		int		parseStartLine(std::string &msg) {
			int	start, end;

			this->parseMethod(start, end, msg);
			this->parseTarget(start, end, msg);
			this->parseIsHTTP(start, end, msg);
			this->parseHTTPVersion(start, end, msg);
			return (start + 5);
		}

		void	parseMethod(int &start, int &end, std::string &msg) {
			start = 0;
			end = msg.find(' ');
			if (end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request msg");
				data.Method = msg.substr(start, end);
				if (data.Method.compare("GET") == 0 ||
					data.Method.compare("POST") == 0 ||
					data.Method.compare("PUT") == 0 ||
					data.Method.compare("PATCH") == 0 ||
					data.Method.compare("DELETE") == 0 ||
					data.Method.compare("HEAD") == 0 ||
					data.Method.compare("CONNECT") == 0 ||
					data.Method.compare("OPTIONS") == 0 ||
					data.Method.compare("TRACE") == 0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "HTTP Method parsing error in request msg");
		}

		void	parseTarget(int &start, int &end, std::string &msg) {
			std::string target;
			start = end + 1;
			end = msg.find(' ', start);
			if (end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no URI in Request msg");
			target = msg.substr(start, end - start);
			if (target.length() > 0 && target[0] == '/') {
				int dir_pos = target.find_last_of("/");
				int file_pos = target.find_last_of(".");
				size_t	query_pos = target.find_last_of("?");

				// /css/style.css 일 경우 check 해보아야함
				if (dir_pos < file_pos) {
					data.uri_dir = target.substr(0, dir_pos);
					if (msg.substr(file_pos, 3).compare("php") == 0) {
						data.isCGI = true;
						data.uri_file = target.substr(dir_pos + 1, query_pos - dir_pos - 1);
						data.query_string = target.substr(query_pos + 1);
					}
					else {
						data.isCGI = false;
						data.uri_file = target.substr(dir_pos + 1);
					}
				}
				else {
					data.isCGI = false;
					data.uri_dir = target;
					data.uri_file = config.getConfig("index");
				}
				return ;
			}
			throw ErrorHandler(__FILE__, __func__, __LINE__, "URI parsing error in request msg");
		}

		void	parseIsHTTP(int &start, int &end, std::string &msg) {
			start = end + 1;
			end = msg.find('/', start);
			std::string isHTTP;
			if (end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol information in Request msg");
			isHTTP = msg.substr(start, end - start + 1);

			if (isHTTP.compare("HTTP/") == 0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request msg is not HTTP");
		}

		void	parseHTTPVersion(int &start, int &end, std::string &msg) {
			start = end + 1;
			std::string HTTP_ver;
			end = msg.find("\r\n", start);
			if (end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol version in Request msg");
			HTTP_ver = msg.substr(start);

			if (HTTP_ver != "1.1")
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request msg is not HTTP");
		}

		void	printStartLine(void) {
			std::cout << "[REQUEST MSG - START LINE]" << std::endl;
			std::cout << "Method : " << data.Method << std::endl;

			std::cout << "uri_dir : " << data.uri_dir << std::endl;
			std::cout << "uri_file : " << data.uri_file << std::endl;
			std::cout << "query_string : " << data.query_string << std::endl;
			std::cout << "isCGI : " << data.isCGI << std::endl;
		}

		/** Header Field **/
		int		parseHeaderField(std::string &msg, int pos) {
			int start = pos, last;
			std::string key, value;

			// header_field가 없는 경우
			if ((int)msg.length() == start)
				return (start + 2);

			while (true) {
				last = msg.find(':', start);
				key = msg.substr(start, last - start);
				start = last + 1;
				if (msg.at(last + 1) == ' ')
					start++;
				last = msg.find("\r\n", start);
				value = msg.substr(start, last - start);
				data.header_field.insert(std::pair<std::string, std::string>(key, value));
				start = last + 2;
				// header_field가 끝나는 경우 (CRLF)
				if (msg.at(start) == '\r' && msg.at(start + 1) == '\n')
					break;
			}
			return (start + 2);
		}

		void	printHeaderField(void) {
			std::cout << "[REQUEST MSG - HEADER FIELD]" << std::endl;
			std::map<std::string, std::string>::iterator it;

			for (it = data.header_field.begin(); it != data.header_field.end(); it++)
				std::cout << it->first << " : " << it->second << std::endl;
		}

		/** Body **/
		void	parseBody(std::string &msg, int pos) {
			int start = pos;
			int last;
			std::string key, value;

			// body가 없는 경우
			if (data.header_field.find("Content-Length") == data.header_field.end() && data.header_field["Content-Length"] == "0")
				return ;

			// body가 있는 경우
			last = msg.find("\r\n", start);
			data.message_body = msg.substr(start, last - start);
			if (data.isCGI)
				write(cgi.getInputPair(), data.message_body.c_str(), data.message_body.length());
		}

		void	printBody(void) {
			std::cout << "[REQUEST MSG - BODY]" << std::endl;
			std::cout << data.message_body << std::endl;
		}
};

#endif
