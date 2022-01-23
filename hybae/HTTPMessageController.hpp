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

// HTTP Message Class
// start_line   : Request/Response에 따라 다르게 구성되어있음.
//							  두 가지 상태에 따라 HTTPMessage 클래스를 상속하여 사용
// header_field : Request/Response 동일
// message_body : Request/Response 동일

class HTTPMessage {
	protected:
		std::string														start_line;
		std::map<std::string, std::string>		header_field;
		std::string														message_body;

	public:
		// TODO: body-message가 있을 경우를 대비하여 CRLF 이후의 위치를 반환하도록 수정
		// header_field를 파싱하여 map에 저장하는 함수
		// message : 클라이언트 측에서 전송하는 데이터
		// pos : start_line을 parsing하고 난 이후의 위치
		void					parseHeaderField(std::string &message, int pos) {
			int start = pos, last;
			std::string key, value;

			// header_field가 없는 경우
			if (message.length() == start)
				return ;

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
			return;
		}

		std::string		getStartLine() {
			return (start_line);
		}

		// TEST를 위해 header_field의 키-값 쌍을 출력하는 함수
		void					getHeaderField() {
			std::map<std::string, std::string>::iterator iter;
			iter = header_field.begin();
			while (iter != header_field.end()) {
				std::cout << iter->first << " : " << iter->second << std::endl;
				iter++;
			}
			return ;
		}
};

// Response Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class ResponseMessage : public HTTPMessage {
	protected:
		double				http_version;
		int						status_code;
		std::string		reason_phrase;

	public:

		void					setHttpVersion(double version) {
			http_version = version;
			return ;
		}

		void					setStatusCode(int code) {
			status_code = code;
			return ;
		}
	
		void					setReasonPhrase() {
			switch(status_code) {
				case 200:
					reason_phrase += "OK";
					break;
				case 403:
					reason_phrase += "Forbidden";
					break;
				case 404:
					reason_phrase += "Not Found";
					break;
				default:
					reason_phrase += "ERROR";
					break;
			}
			return ;
		}

		// 파일 전송 확인을 위한 임시 값 설정
		void					setHeaderField() {
			
			header_field.insert(std::make_pair<std::string, std::string>("Content-Type", "text/html"));
			header_field.insert(std::make_pair<std::string, std::string>("Content-Length", "138"));
			header_field.insert(std::make_pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		void					setStartLine() {
			std::stringstream ss_http_version; 
			ss_http_version << http_version;
			std::stringstream ss_status_code; 
			ss_status_code << status_code;

			start_line += "HTTP/";
			start_line += ss_http_version.str();
			start_line += " ";
			start_line += ss_status_code.str();
			start_line += " ";
			start_line += reason_phrase;
			return ;
		}

		std::string		makeResponseMessage() {
			std::map<std::string, std::string>::iterator iter;
			std::string rtn;

			rtn += start_line;
			rtn += "\r\n";
			iter = header_field.begin();
			while (true) {
				rtn += iter->first;
				rtn += ": ";
				rtn += iter->second;
				rtn += "\r\n";
				iter++;
				if (iter == header_field.end())
					break;
			}
			rtn += "\r\n";
			rtn += message_body;
			return (rtn);
		}

		// FIXME: rdbuf로 파일을 전체 받아오는 경우, 파일의 크기만큼 메모리가 할당된다.
		//				오버플로우의 위험이 있음.
		//				buffer를 만들고 그 크기만큼 쪼개서 읽고, 전송을 반복하는 방법으로 수정 가능
		void						setMessageBody(std::string uri) {
			std::ifstream file(uri);
			std::stringstream ss;

			ss << file.rdbuf();
			message_body = ss.str();
			return ;
		}
};

// Request Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class RequestMessage : public HTTPMessage {
	public:
		std::string	method;
		std::string	request_target;
		std::string	isHTTP;
		double			http_version;

		// Method가 올바른지 판단하기 위한 함수
		int					checkMethod() {
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

		// uri가 올바른지 판단하기 위한 함수
		int					checkTarget() {
			if (request_target.length() > 0 && request_target[0] == '/')
				return (0);
			return (-1);
		}

		// HTTP 형식인지 판단하기 위한 함수
		int					checkIsHTTP() {
			if (isHTTP.compare("HTTP/") == 0)
				return (0);
			return (-1);
		}

		// HTTP version이 올바른지 판단하기 위한 함수
		int					checkHTTPVersion() {
			if (http_version == 1.0 ||
					http_version == 1.1 ||
					http_version == 2.0)
				return (0);
			return (-1);
		}

		// start_line을 parsing하여 각 변수에 담는 함수
		// start_line의 데이터는 필수이므로 각 데이터에 대한 예외 처리
		int					parseStartLine(std::string& message) {
			int	start = 0;
			int last;
			last = message.find(' ');
			method = message.substr(start, last);
			if (checkMethod() == -1)
				return (-1);
			start = last + 1;
			last = message.find(' ', start);
			request_target = message.substr(start, last - start);
			if (checkTarget() == -1)
				return (-1);
			start = last + 1;
			last = message.find('/', start);
			isHTTP = message.substr(start, last - start + 1);
			if (checkIsHTTP() == -1)
				return (-1);
			start = last + 1;
			http_version = atof(message.substr(start).c_str());
			if (checkHTTPVersion() == -1)
				return (-1);
			// header_field parsing을 위해 start_line 이후 문자의 인덱스 반환
			return (start + 5);
		}

		// TEST를 위해 start_line의 데이터를 출력하는 함수
		void				getStartLine() {
			std::cout << "Method\t: " << method << std::endl;
			std::cout << "Target\t: " << request_target << std::endl;
			std::cout << "isHTTP\t: " << isHTTP << std::endl;
			std::cout << "Version\t: " << http_version << std::endl;
		}

		static int	parsingRequestMessage(int socket_client, RequestMessage* requestMessage) {
			int n;
			char charMessage[BUFSIZ];
			read(socket_client, &charMessage, BUFSIZ);
			std::string stringMessage(charMessage);
			if ((n = requestMessage->parseStartLine(stringMessage)) == -1) {
				std::cout << "PARSE ERROR" << std::endl;
				close(socket_client);
				return (-1);
			}
			requestMessage->parseHeaderField(stringMessage, n);
			return (0);
		}

		// Test를 위한 RequestMessage 및 client 주소 출력
		static void			printRequestMessage(RequestMessage requestMessage, struct sockaddr_in	address_client) {
			requestMessage.getStartLine();
			requestMessage.getHeaderField();
			std::cout << "CLI ADDR\t: " << inet_ntoa(address_client.sin_addr) << std::endl;
			std::cout << "CLI PORT\t: " << ntohs(address_client.sin_port) << std::endl;
			return ;
		}
};

#endif