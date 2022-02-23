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
#include "./../ServerProcessController/ServerProcess.hpp"
#include "./../ParsingController/MIMEController.hpp"
#include "./../ErrorHandler/ErrorHandler.hpp"
// #include "./../CGI/CGIProcess.hpp"

extern ConfigController config;
extern MIMEController mime;

// HTTP Message Class
// start_line   : Request/Response에 따라 다르게 구성되어있음.
//							  두 가지 상태에 따라 HTTPMessage 클래스를 상속하여 사용
// header_field : Request/Response 동일
// message_body : Request/Response 동일

class HTTPMessage {
	protected:
		std::string								start_line;
		std::map<std::string, std::string>		header_field;
		std::string								message_body;
		char*									binary_body;
		bool									isCGI;

	public:
		std::string		getMessageBody()	{return message_body; }
		std::string		getStartLine()		{ return (start_line); }

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

class RequestMessage : public HTTPMessage {
	private:
		std::string	method;
		std::string	request_target;
		std::string	uri_dir;
		std::string	uri_file;
		std::string	query_string;
		bool		isCGI;
		std::string	isHTTP;
		double		http_version;

	public:

		std::string		getMethod()				{ return method; }
		std::string		getRequestTarget()		{ return request_target; }
		std::string		getUriDir()				{ return uri_dir; }
		std::string		getUriFile()			{ return uri_file; }
		std::string		getQueryString()		{ return query_string; }
		double			getHttpVersion()		{ return http_version; }
		bool			getIsCGI()				{ return isCGI; }

		void			setIsCGI(bool flag)		{ this->isCGI = flag; }

		void    printVar() {
            std::cout << __func__ << std::endl;
            std::cout << "method : " << method << std::endl;
            std::cout << "request_target : " << request_target << std::endl;
            std::cout << "uri_dir : " << uri_dir << std::endl;
            std::cout << "uri_file : " << uri_file << std::endl;
			std::cout << "query_string : " << query_string << std::endl;
			std::cout << "isCGI : " << isCGI << std::endl;
            std::cout << "isHTTP : " << isHTTP << std::endl;
            std::cout << "http_version : " << http_version << std::endl;
            std::cout << "----------------------" << std::endl;
        }

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
		void					parseMethod(int* start, int* end, std::string& message) {
			*start = 0;
			*end = message.find(' ');
			if (*end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP Method in Request Message");
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
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "HTTP Method parsing error in request message");
		}

		// HTTP uri parsing 및 에러 처리
		void					parseTarget(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find(' ', *start);
			if (*end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no URI in Request Message");
			this->request_target = message.substr(*start, *end - *start);
			if (request_target.length() > 0 && request_target[0] == '/') {
				int dir_pos = request_target.find_last_of("/");
				int file_pos = request_target.find_last_of(".");
				size_t	query_pos = request_target.find_last_of("?");
				if (dir_pos < file_pos) {
					this->uri_dir = request_target.substr(0, dir_pos);
					if (query_pos == std::string::npos) {
						this->isCGI = false;
						this->uri_file = request_target.substr(dir_pos + 1);
					}
					else {
						this->isCGI = true;
						this->uri_file = request_target.substr(dir_pos + 1, query_pos - dir_pos - 1);
						this->query_string = request_target.substr(query_pos + 1);
					}
				}
				else {
					this->isCGI = false;
					this->uri_dir = request_target;
					this->uri_file = config.getConfig("index");
				}
				return ;
			}
			throw ErrorHandler(__FILE__, __func__, __LINE__, "URI parsing error in request message");
		}

		// HTTP protocol check 및 에러 처리
		void					parseIsHTTP(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find('/', *start);
			if (*end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol information in Request Message");
			this->isHTTP = message.substr(*start, *end - *start + 1);

			if (isHTTP.compare("HTTP/") == 0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
		}

		// TODO: double 타입의 값을 소수점 첫째 자리까지만 들어오도록 수정할 것
		// HTTP version check 및 에러 처리
		void					parseHTTPVersion(int* start, int* end, std::string& message) {
			*start = *end + 1;
			*end = message.find("\r\n", *start);
			if (*end == -1)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "There is no HTTP protocol version in Request Message");
			http_version = atof(message.substr(*start).c_str());

			if (http_version == 1.0 ||
					http_version == 1.1 ||
					http_version == 2.0)
				return ;
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Request Message is not HTTP");
		}

		// start_line을 parsing하여 각 변수에 담는 함수
		// start_line의 데이터는 필수이므로 각 데이터에 대한 예외 처리
		// header_field parsing을 위해 start_line 이후 문자의 인덱스 반환
		int						parseStartLine(std::string& message) {
			int	start, end;

			this->parseMethod(&start, &end, message);
			this->parseTarget(&start, &end, message);
			this->parseIsHTTP(&start, &end, message);
			this->parseHTTPVersion(&start, &end, message);
			return (start + 5);
		}

		// TODO: POST의 경우 message body도 parsing 필요함
		// Request Message parsing
		// start line, header field
		void					parsingRequestMessage(int fd, std::string& message) {
			CGIProcess cgi;
			int n;

			if ((n = this->parseStartLine(message)) == -1) {
				close(fd);
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Start line parsing error in Request message");
			}

			n = this->parseHeaderField(message, n);

			if (this->isCGI)
				cgi.CGIprocess();

			// content length 헤더가 있는지 판별하여 body가 있는지 확인한다.
			if (this->method == "POST") {
				// content length 만큼만 읽고 response message 작성(write) 으로 바로 넘어가도록 작성해야한다.
				// client가 data를 계속 보내지 않고 있었을 때를 대비하여 timeout 을 설정 하여야 한다. (config에 기재되어있음)
				std::cout << "body test : " << (message.c_str() + n) << std::endl;
				// cgi 라면 cgi.getInputPair() 로 fd 를 받아서 처리
				if (this->isCGI)
					write(cgi.getInputPair(), message.c_str(), message.length());
			}
			printVar();
			return ;
		}
};

class ResponseMessage : public HTTPMessage {
	protected:
		double			http_version;
		int				status_code;
		std::string		reason_phrase;
		std::string		extension;
		bool			is_binary;
		int				binary_size;
		

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
			start_line += std::to_string(http_version).substr(0, 3);
			start_line += " ";
			start_line += std::to_string(status_code);
			start_line += " ";
			start_line += reason_phrase;
			return ;
		}

		// start line, header field, message body를 합쳐 response message 반환
		std::string		makeResponseMessage(bool isCGI) {
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
			if (isCGI == false) {
				rtn += "\r\n";
				rtn += message_body;
			}
			else
				rtn += message_body;
			return (rtn);
		}

		// FIXME: static page 폴더 내 index.html 파일 파싱 시, 크기가 큰 파일들 파싱 안 됨
		//				파일을 전체 받아오는 경우, 파일의 크기만큼 메모리가 할당된다.
		//				오버플로우의 위험이 있음.
		//				buffer를 만들고 그 크기만큼 쪼개서 읽고, 전송을 반복하는 방법으로 수정 가능
		//				binary 파일이 string 타입에 대입 시 제대로 들어가는지 확인 필요
		void						setMessageBody(RequestMessage* requestMessage, std::string uri) {
			std::cout << "URI : " << uri << std::endl;

            //요청 메시지에서 cgi(php)가 발견되었다면
            //read로 데이터를 읽어서 저장한다
			if (requestMessage->getIsCGI()) {
				// CGIProcess	cgi;
				// cgi.setEnvp(requestMessage);
				// cgi.CGIprocess();
				// std::cout << "cgi.getOutputPair() -> " << cgi.getOutputPair() << std::endl;
				// char	buffer[1024];
				// int n;
				// while ((n = read(cgi.getOutputPair(), buffer, 1024)) > 0) {
				// 	buffer[n] = '\0';
				// 	message_body += std::string(buffer);
				// 	std::cout << "buf -> " << message_body << std::endl;
				// }
                // if (n < 0)
                //     throw (ErrorHandler(__FILE__, __func__, __LINE__, "ERROR : READ CGI ERROR"));
				// // cgi 에서 받은 메세지 리팩토링
				std::cout << message_body << std::endl;
			} 
            else {
				std::ifstream file(uri);
				std::string		line;

				if (file.is_open()) {
					file.seekg(0, std::ios::end);
					int size = file.tellg();
					line.resize(size);
					file.seekg(0, std::ios::beg);
					file.read(&line[0], size);
					message_body = line;
				}
				while (std::getline(file, line)) {
					message_body = message_body + line + "\n";
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
			int					flag = 0;
			DIR					*dp;
			struct				dirent *dir;

			if (requestMessage->getIsCGI() == false)
				*uri = config.getConfig("root") + requestMessage->getUriDir();
			else
				*uri = "." + requestMessage->getUriDir();
			std::cout << "URI : " << *uri << std::endl;
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
			std::string			uri;
			int					flag;

			flag = responseMessage.isDirOrFile(requestMessage, &uri);

			responseMessage.setStartLine(flag, requestMessage->getHttpVersion());

			responseMessage.setMessageBody(requestMessage, uri);

			responseMessage.setResponseHeaderField();
			std::string rtn = responseMessage.makeResponseMessage(requestMessage->getIsCGI());
			return (rtn);
		}
};

#endif
