#ifndef HTTPMESSAGECONTROLLER_HPP
# define HTTPMESSAGECONTROLLER_HPP

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
// #include "ServerProcess.hpp"
#include "MIMEController.hpp"
#include "ErrorHandler.hpp"
#include "ConfigController.hpp"
// #include "./../CGI/CGIProcess.hpp"

extern ConfigController config;
extern MIMEController mime;

// HTTP Message Class
// start_line   : Request/Response에 따라 다르게 구성되어있음.
//							  두 가지 상태에 따라 HTTPMessage 클래스를 상속하여 사용
// header_field : Request/Response 동일
// message_body : Request/Response 동일

class HTTPMessageController {
	protected:
		std::string								start_line;
		std::map<std::string, std::string>		header_field;
		std::string								message_body;
		char*									binary_body;
		bool									isCGI;

	public:
		std::string		getMessageBody()	{return message_body; }
		std::string		getStartLine()		{ return (start_line); }
		std::string		getHeaderField(std::string key) { return (header_field[key]); }

		// TODO: body-message가 있을 경우를 대비하여 CRLF 이후의 위치를 반환하도록 수정
		// header_field를 파싱하여 map에 저장하는 함수
		// message : 클라이언트 측에서 전송하는 데이터
		// pos : start_line을 parsing하고 난 이후의 위치
		int						parseHeaderField(std::string &message, int pos) {
			int start = pos, last;
			std::string key, value;

			// header_field가 없는 경우
			if ((int)message.length() == start)
				return (start + 2);

			while (true) {
				last = message.find(':', start);
				key = message.substr(start, last - start);
				start = last + 1;
				if (message.at(last + 1) == ' ')
					start++;
				last = message.find("\r\n", start);
				value = message.substr(start, last - start);
				header_field.insert(std::pair<std::string, std::string>(key, value));
				start = last + 2;
				// header_field가 끝나는 경우 (CRLF)
				if (message.at(start) == '\r' && message.at(start + 1) == '\n')
					break;
			}
			return (start + 2);
		}

		// 파라미터로 들어오는 key-value 쌍을 header_field에 삽입하는 메소드
		void					setHeaderField(std::string key, std::string value) {
			header_field.insert(std::make_pair<std::string, std::string>(key, value));
			return ;
		}

		// TEST를 위해 header_field의 키-값 쌍을 출력하는 함수
		void					printHeaderField() {
			std::map<std::string, std::string>::iterator iter;
			iter = header_field.begin();
			while (iter != header_field.end()) {
				std::cout << iter->first << " : " << iter->second << std::endl;
				iter++;
			}
			return ;
		}
};

#endif
