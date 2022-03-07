#ifndef REQUESTMESSAGE_HPP
# define REQUESTMESSAGE_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include "HTTPData.hpp"
#include "ErrorHandler.hpp"
#include "ConfigController.hpp"
#include "ConfigBlocks.hpp"

extern NginxConfig::GlobalConfig _config;

class RequestMessage {
	private:
		typedef enum	e_Seq {
			START_LINE,
			HEADER_FIELD,
			MESSAGE_BODY
		}				Seq;
		HTTPData*	data;
		int			parsing_pointer;
		std::string	message;
		Seq			seq;
	public:
		RequestMessage(HTTPData* _data) : data(_data), parsing_pointer(0), message(""), seq(START_LINE) { }

		void	setMessage(char* buffer) {
			std::string	temp(buffer);
			this->message += temp;
			return ;
		}

		void	resetMessage() {
			this->message = this->message.substr(this->parsing_pointer);
			return ;
		}

/*
		void	parsingRequestMessage(int fd, std::string &msg) {

			this->parseStartLine(msg);
			printStartLine();

			// if (data->isCGI) {
			// 	cgi.setEnvp(this);
			// 	cgi.run();
			// }

			// request msg - header_field
			this->parseHeaderField(msg);
			printHeaderField();

			// request msg - body
			this->parseBody(msg);
			printBody();

			if (this->data->isCGI) {
				// message body가 있다면
				if (this->data->header_field.find("Content-Length") != this->data->header_field.end() && this->data->header_field["Content-Length"] != "0") {
					std::cout << "CGI DATA : " << msg.c_str() << std::endl;
					//write(cgi.getInputPair(), msg.c_str(), msg.length());
				}
				char buf[1024];
				//read(cgi.getOutputPair(), buf, 1024);
				std::cout << "buf : " << buf << std::endl;
			}
		}
*/
		/** Header **/

		// message 내에 start line이 모두 담겨져있는지 확인하는 메소드
		// start line 파싱이 가능하면 true, 불가능하면 false 반환
		bool	parsingRequestMessage() {
			if (this->seq == START_LINE) {
				std::cout << "START LINE" << std::endl;
				int	start_line_pos = int(this->message.find("\r\n", 0));
				std::string start_line_msg = this->message.substr(0, start_line_pos);
				if (start_line_pos != -1) {
					parseStartLine(start_line_msg);
					resetMessage();
					this->seq = HEADER_FIELD;
					//printStartLine();
				}
			}
			else if (this->seq == HEADER_FIELD) {
				std::cout << "HEADER_FIELD" << std::endl;
				if (int(this->message.find("\r\n\r\n", 0)) != -1) {
					std::cout << "HEADER_FIELD PARSING" << std::endl;
					parseHeaderField(this->message);
					resetMessage();
					this->seq = MESSAGE_BODY;
					printHeaderField();
				}
			}
			else if (this->seq == MESSAGE_BODY) {
				if (data->header_field["Content-Length"] != "0" &&
					int(this->message.length()) == atoi(data->header_field["Content-Length"].c_str())) {
					std::cout << "MESSAGE_BODY PARSING" << std::endl;
					parseMessageBody(this->message);
					return (true);
				}
			}
			else {
				return (false);
			}
			return (false);
		}

		void	parseStartLine(std::string &msg) {
			int	start, end;
			this->parseMethod(start, end, msg);
			this->parseTarget(start, end, msg);
			this->parseHttpVersion(start, end, msg);
			this->parsing_pointer = start + 5;
		}

		void	parseMethod(int &start, int &end, std::string &msg) {
			start = 0;
			end = msg.find(' ');
			if (end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request msg");
				data->method = msg.substr(start, end);
				if (data->method.compare("GET") == 0 ||
					data->method.compare("POST") == 0 ||
					data->method.compare("PUT") == 0 ||
					data->method.compare("PATCH") == 0 ||
					data->method.compare("DELETE") == 0 ||
					data->method.compare("HEAD") == 0 ||
					data->method.compare("CONNECT") == 0 ||
					data->method.compare("OPTIONS") == 0 ||
					data->method.compare("TRACE") == 0)
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

			int query_pos = int(target.find_last_of("?"));
			if (query_pos != -1) {
				data->query_string = target.substr(query_pos + 1);
				target = target.substr(0, target.length() - data->query_string.length() - 1);
				
			}
			int	extension_pos = target.find_last_of(".");
			this->data->isCGI = false;
			if (extension_pos != -1) {
				data->file_extension = target.substr(extension_pos + 1);
				int	file_pos = target.find_last_of("/");
				data->uri_file = target.substr(file_pos + 1);
				target = target.substr(0, target.length() - data->uri_file.length());
				for (int i = 0; i < int(_config._http._server[this->data->server_block]._location.size()); i++) {
					std::string temp = _config._http._server[this->data->server_block]._location[i]._location;
					temp = temp.substr(1, temp.length() - 2);
					if (this->data->file_extension.compare(temp) == 0) {
						this->data->isCGI = true;
						break;
					}
				}
			}
			data->uri_dir = target;
			this->parsing_pointer = end + 2;
			resetMessage();
		}

		void	parseHttpVersion (int &start, int &end, std::string &msg) {
			start = end + 1;
			std::string httpCheck = msg.substr(start);
			if (httpCheck.compare("HTTP/1.1") == 0) {
				this->data->http_version = 1.1;
				start += httpCheck.length();
				return ;
			}
			else
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Not HTTP 1.1");
		}

		void	printStartLine(void) {
			std::cout << "[REQUEST MSG - START LINE]" << std::endl;
			std::cout << "Method : " << data->method << std::endl;
			std::cout << "uri_dir : " << data->uri_dir << std::endl;
			std::cout << "uri_file : " << data->uri_file << std::endl;
			std::cout << "query_string : " << data->query_string << std::endl;
			std::cout << "file_extension : " << data->file_extension << std::endl;
			std::cout << "http_version : " << data->http_version << std::endl;
			std::cout << "isCGI : " << data->isCGI << std::endl;
		}

		/** Header Field **/
		void	parseHeaderField(std::string &msg) {
			int start = this->parsing_pointer, end;
			std::string key, value;

			// header_field가 없는 경우
			if ((int)msg.length() == start) {
				this->parsing_pointer = start + 2;
				return ;
			}
			while (true) {
				end = msg.find(':', start);
				key = msg.substr(start, end - start);
				start = end + 1;
				if (msg.at(end + 1) == ' ')
					start++;
				end = msg.find("\r\n", start);
				value = msg.substr(start, end - start);
				data->header_field.insert(std::pair<std::string, std::string>(key, value));
				start = end + 2;
				// header_field가 끝나는 경우 (CRLF)
				if (msg.at(start) == '\r' && msg.at(start + 1) == '\n')
					break;
			}
			this->parsing_pointer = start + 2;
			return ;
		}

		void	printHeaderField(void) {
			std::cout << "[REQUEST MSG - HEADER FIELD]" << std::endl;
			std::map<std::string, std::string>::iterator it;

			for (it = data->header_field.begin(); it != data->header_field.end(); it++)
				std::cout << it->first << " : " << it->second << std::endl;
		}

		/** Body **/
		void	parseMessageBody(std::string &msg) {
			int start = this->parsing_pointer, end;
			std::string key, value;

			// body가 없는 경우
			if (data->header_field.find("Content-Length") == data->header_field.end() && data->header_field["Content-Length"] == "0")
				return ;

			// body가 있는 경우
			end = msg.find("\r\n", start);
			data->message_body = msg.substr(start, end - start);
			//if (data->isCGI)
				//write(cgi.getInputPair(), data->message_body.c_str(), data->message_body.length());
		}

		void	printBody(void) {
			std::cout << "[REQUEST MSG - BODY]" << std::endl;
			std::cout << data->message_body << std::endl;
		}
};

#endif
