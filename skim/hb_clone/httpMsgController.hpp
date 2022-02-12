#ifndef HTTPMSGCONTROLLER_HPP
# define HTTPMSGCONTROLLER_HPP

#include <string>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <utility>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

class HTTPMsg {
	protected:
		std::map<std::string, std::string>	header_field;
		std::string							start_line;
		std::string							msg_body;
	public:
		std::string	getStartLine(void) {
			return (this->start_line);
		}

		std::string	getMsgBody(void) {
			return (msg_body);
		}

		void		printHeaderField(void) {
			std::map<std::string, std::string>::iterator	it;
			for (it = header_field.begin(); it != header_field.end(); it++) {
				std::cout << it->first << " : " << it->second << std::endl;
			}
			return ;
		}

		// pos : start_line을 파싱하고 난 이후의 위치
		void		parseHeaderField(std::string &msg, int pos)
		{
			int	start = pos, last;
			std::string	key, value;

			// header_field가 없는 경우
			if (msg.length() == start)
				return ;

			while (1) {
				last = msg.find(":", start);
				key = msg.substr(start, last - start);
				start = last + 1;
				if (msg.at(last + 1) == ' ')
					start++;
				last = msg.find("\r\n", start);
				value = msg.substr(start, last - start);
				header_field.insert(std::make_pair(key, value));
				start = last + 2;

				// header_field가 끝나는 경우 (CRLF)
				if (msg.at(start) == '\r' && msg.at(start + 1) == '\n')
					break ;
			}
			return ;
		}

		void		setHeaderField(std::string key, std::string value) {
			header_field.insert(std::make_pair<std::string, std::string>(key, value));
		}
};

#endif
