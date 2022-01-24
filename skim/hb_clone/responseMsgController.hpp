#ifndef RESPONSECONTROLLER_HPP
# define RESPONSECONTROLLER_HPP

#include "httpMsgController.hpp"


// http 응답 메세지 형태 ex)
// HTTP/1.1 200 OK

// Date: Sat, 09 Oct 2010 14:28:02 GMT
// Server: Apache
// Last-Modified: Tue, 01 Dec 2009 20:18:22 GMT
// ETag: "51142bc1-7449-479b075b2891b"

// Accept-Ranges: bytes
// Content-Length: 29769 // 무엇에 대한 길이인가...?
// Content-Type: text/html

class responseMsg : public HTTPMsg {
	protected:
		double		http_version;
		int			status_code;
		std::string	reason;
	public:
		void		setHttpVersion(double version) {
			this->http_version = version;
		}

		void		setStatusCode(int code) {
			this->status_code = code;
		}

		void		setReason(void) {
			switch(status_code) {
				case 200:
					this->reason += "OK";
					break ;
				case 403:
					this->reason += "Forbidden";
					break ;
				case 404:
					this->reason += "Not Found";
					break ;
				default:
					this->reason += "Error";
					break ;
			}
		}

		// Accept-Ranges: bytes
		// Content-Length: 29769 // 무엇에 대한 길이인가...?
		// Content-Type: text/html
		void		setHeaderField(void) {
			header_field.insert(std::make_pair<std::string, std::string>("Content-Type", "text/html"));
			header_field.insert(std::make_pair<std::string, std::string>("Content-Length", "138"));
			header_field.insert(std::make_pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		// HTTP/1.1 200 OK
		void		setStartLine(void) {
			std::stringstream	ss_http_version; // string에서 각 정보를 추출하여 다루기 좋은 라이브러리
			ss_http_version << http_version;
			std::stringstream	ss_status_code;;
			ss_status_code << status_code;

			start_line += "HTTP/";
			start_line += ss_http_version.str();
			start_line += " ";
			start_line += ss_status_code.str();
			start_line += " ";
			start_line += reason;
		}

		std::string	makeResponseMsg(void) {
			std::map<std::string, std::string>::iterator	it;
			std::string	rtn;

			rtn += start_line;
			rtn += "\r\n";
			for (it = header_field.begin(); it != header_field.end(); it++) {
				rtn += it->first;
				rtn += ": ";
				rtn += it->second;
				rtn += "\r\n";
			}
			rtn += "\r\n";
			rtn += msg_body;
			return (rtn);
		}

		// rdbuf에 대하여 좀 더 조사해볼 것!
		void		setMsgBody(std::string uri) {
			std::ifstream		file(uri);
			std::stringstream	ss_file;

			ss_file << file.rdbuf();
			msg_body = ss_file.str();
		}
};

#endif
