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
#include "./ServerProcess.hpp"
#include "./MIMEController.hpp"

extern ConfigController config;
extern MIMEController mime;

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
		std::string		getMessageBody()	{return message_body; }
		std::string		getStartLine()		{ return (start_line); }

		// TODO: body-message가 있을 경우를 대비하여 CRLF 이후의 위치를 반환하도록 수정
		// header_field를 파싱하여 map에 저장하는 함수
		// message : 클라이언트 측에서 전송하는 데이터
		// pos : start_line을 parsing하고 난 이후의 위치
		void					parseHeaderField(std::string &message, int pos) {
			int start = pos, last;
			std::string key, value;

			// header_field가 없는 경우
			if ((int)message.length() == start)
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

// Request Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class RequestMessage : public HTTPMessage {
	private:
		std::string	method;
		std::string	request_target;
		std::string	uri_dir;
		std::string	uri_file;
		std::string	isHTTP;
		double			http_version;

	public:

		std::string		getMethod()					{ return method; }
		std::string		getRequestTarget()			{ return request_target; }
		std::string		getUriDir()					{ return uri_dir; }
		std::string		getUriFile()				{ return uri_file; }
		double				getHttpVersion()		{ return http_version; }

		// reset R equest message 
		void					resetMessage() {
			start_line = "";
			header_field.clear();
			message_body = "";
			method = "";
			request_target = "";
			uri_dir = "";
			uri_file = "";
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
			std::cout << message.substr(*start, *end) << std::endl;
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
				if (dir_pos < file_pos) {
					this->uri_dir = request_target.substr(0, dir_pos);
					this->uri_file = request_target.substr(dir_pos + 1);
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
		
		// TODO: double 타입의 값을 소수점 첫째 자리까지만 들어오도록 수정할 것
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
				std::cout << "PARSE ERROR" << std::endl;
				close(fd);
				return (-1);
			}
			this->parseHeaderField(message, n);
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

// Response Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class ResponseMessage : public HTTPMessage {
	protected:
		double				http_version;
		int						status_code;
		std::string		reason_phrase;
		std::string		extension;

	public:

		// reset response message
		void					resetMessage() {
			start_line = "";
			header_field.clear();
			message_body = "";
			http_version = 0;
			status_code = 0;
			reason_phrase = "";
			return ;
		}

		void					setHttpVersion(double version)			{ http_version = version; }
		void					setStatusCode(int code)							{ status_code = code; }
		void					setExtension(std::string extension)	{ this->extension = extension; }

		// status code에 따른 reason phrase값 설정
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

		// TODO: binary 파일의 경우, content-length 값 0으로 설정되는 점 수정
		// header field 값 설정
		void					setResponseHeaderField() {

			header_field.insert(std::make_pair<std::string, std::string>("Content-Type", mime.getMIME(this->extension)));			
			this->setHeaderField("Content-Length", std::to_string(this->getMessageBody().length()));
			header_field.insert(std::make_pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		// startline 설정
		void					setStartLine(int status_code, double http_version) {
			this->setHttpVersion(http_version);
			this->setStatusCode(status_code);
			this->setReasonPhrase();

			start_line += "HTTP/";
			start_line += std::to_string(http_version);
			start_line += " ";
			start_line += std::to_string(status_code);
			start_line += " ";
			start_line += reason_phrase;
			return ;
		}

		// start line, header field, message body를 합쳐 response message 반환
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

		// FIXME: static page 폴더 내 index.html 파일 파싱 시, 크기가 큰 파일들 파싱 안 됨
		//				파일을 전체 받아오는 경우, 파일의 크기만큼 메모리가 할당된다.
		//				오버플로우의 위험이 있음.
		//				buffer를 만들고 그 크기만큼 쪼개서 읽고, 전송을 반복하는 방법으로 수정 가능
		//				binary 파일이 string 타입에 대입 시 제대로 들어가는지 확인 필요
		void						setMessageBody(std::string uri) {
			std::cout << "URI : " << uri << std::endl;
			if (extension.compare("jpg") == 0 || extension.compare("ico") == 0) {
				std::cout << "IMAGE" << std::endl;
				//std::ofstream copy;
				std::string temp;
				int pos = uri.find_last_of(".");
				temp = uri.substr(pos - 5);
				//copy.open("./copy_" + temp, std::ios::out | std::ios::binary);
				std::ifstream is(uri, std::ifstream::binary);
				if (is) {
					// seekg를 이용한 파일 크기 추출
					is.seekg(0, is.end);
					int length = (int)is.tellg();
					is.seekg(0, is.beg);

					//만약 length가 타겟파일의 전체 크기라면
					// 방안 1. 일정 크기를 넘어서는지 검사하고, 넘은 파일에 대하여 '고정분할'
					// 대략...
					// if (length >= 기준)
					// {
					// 	int chunkCount = length / 10(임의의 분할값);

					// 	char *buffer = 1000(임의의 버퍼값);

					// 	for(int i = 0; i < chunkCount; i++)
					// 	{
					// 		is.read((char*)buffer, length);
					// 		std::string temp(buffer);
					// 		message_body = temp;
					// 		(is.close());
					// 	}
					// }

					// 방안 2. 버퍼 사이즈를 고정하고, 오프셋에 도달 전까지 계속 read
					// while()
					// {
					// ....
					// }
					

					// malloc으로 메모리 할당
					char *buffer = NULL;

					// read data as a block:
					is.read((char*)buffer, length);
					std::string temp(buffer);
					//copy.write((char*)buffer, length);
					message_body = temp;
					is.close();
				}
			}
			else {
				std::ifstream file(uri);
				std::string		line;
				int	i = 0;
				while (std::getline(file, line)) {
					message_body = message_body + line + "\n";
					i++;
					if (i == 5)
						std::cout << "BODY : \n" << message_body << std::endl;
				}
			}
			return ;
		}

		// uri가 dir인지, file인지 확인하는 메소드
		// 올바르지 않은 디렉토리 : 403
		// 올바른 디렉토리 : 200 + index.html
		// 올바른 파일 : 200
		// 올바르지 않은 파일 : 404
		int			isDirOrFile(RequestMessage* requestMessage, std::string* uri) {
			int						flag = 0;
			DIR						*dp;
			struct				dirent *dir;

			*uri = config.getConfig("root") + requestMessage->getUriDir();
			
			if ((dp = opendir(uri->c_str())) == NULL) {
				std::cout << "DIR Open Error" << std::endl;
				*uri = config.getConfig("root") + "/403.html";
				this->setExtension("html");
				return (403);
			}			
			if (requestMessage->getUriDir().compare("/") != 0)
				*uri = *uri + "/";
			*uri = *uri + requestMessage->getUriFile();
			while((dir = readdir(dp)) != NULL) {
				if (dir->d_ino == 0)
					continue;
				if (strcmp(requestMessage->getUriFile().c_str(), dir->d_name) == 0) {
					std::string extension;
					int pos = requestMessage->getUriFile().find_last_of(".");
					extension = requestMessage->getUriFile().substr(pos + 1);
					this->setExtension(extension);
					flag = 200;
					break;
				}
			}
			
			if (flag != 200) {
				*uri = config.getConfig("root") + "/404.html";
				flag = 404;
				this->setExtension("html");
			}
			closedir(dp);
			return (flag);	
		}

		// response message의 구성 요소들을 가공하여 string 형태로 반환
		static std::string	setResponseMessage(RequestMessage* requestMessage) {
			ResponseMessage 	responseMessage;
			std::string				uri;
			int								flag;

			flag = responseMessage.isDirOrFile(requestMessage, &uri);
			
			responseMessage.setStartLine(flag, requestMessage->getHttpVersion());

			responseMessage.setMessageBody(uri);
			
			responseMessage.setResponseHeaderField();
			std::string rtn = responseMessage.makeResponseMessage();
			return (rtn);
		}
};

#endif