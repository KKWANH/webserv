#ifndef REQUESTMESSAGECONTROLLER_HPP
# define REQUESTMESSAGECONTROLLER_HPP

#include "HTTPMessageController.hpp"
#include "CGIProcess.hpp"

// class HTTPMessage;
class CGIProcess;
// // Request Message class
// // Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
class RequestMessage : public HTTPMessageController {
 	private:
 		std::string	method;
 		std::string	request_target;
 		std::string	uri_dir;
 		std::string	uri_file;
 		std::string	query_string;
 		bool		isCGI;
 		std::string	isHTTP;
 		double		http_version;
		CGIProcess cgi;

 	public:
 		std::string		getMethod();
 		std::string		getRequestTarget();
 		std::string		getUriDir();
 		std::string		getUriFile();
 		std::string		getQueryString();
 		double			getHttpVersion();
 		bool			getIsCGI();			

 		void			setIsCGI(bool flag);
 		void            printVar();

 		void			resetMessage();

// 		// HTTP Method parsing 및 에러 처리
 		void			parseMethod(int* start, int* end, std::string& message);


// 		// HTTP uri parsing 및 에러 처리
 		void					parseTarget(int* start, int* end, std::string& message);


// 		// HTTP protocol check 및 에러 처리
		void					parseIsHTTP(int* start, int* end, std::string& message);


// 		// TODO: double 타입의 값을 소수점 첫째 자리까지만 들어오도록 수정할 것
// 		// HTTP version check 및 에러 처리
 		void					parseHTTPVersion(int* start, int* end, std::string& message);

// 		// start_line을 parsing하여 각 변수에 담는 함수
// 		// start_line의 데이터는 필수이므로 각 데이터에 대한 예외 처리
// 		// header_field parsing을 위해 start_line 이후 문자의 인덱스 반환
 		int						parseStartLine(std::string& message);


// 		// TODO: POST의 경우 message body도 parsing 필요함
// 		// Request Message parsing
// 		// start line, header field
 		void					parsingRequestMessage(int fd, std::string& message);

 };

 #endif
