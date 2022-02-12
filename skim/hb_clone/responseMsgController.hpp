#ifndef RESPONSECONTROLLER_HPP
# define RESPONSECONTROLLER_HPP

#include "./configController.hpp"
#include "./MIMEController.hpp"
#include "./httpMsgController.hpp"


// http 응답 메세지 형태 ex)
// HTTP/1.1 200 OK

// Date: Sat, 09 Oct 2010 14:28:02 GMT
// Server: Apache
// Last-Modified: Tue, 01 Dec 2009 20:18:22 GMT
// ETag: "51142bc1-7449-479b075b2891b"

// Accept-Ranges: bytes
// Content-Length: 29769 // 무엇에 대한 길이인가...?
// Content-Type: text/html

extern configController	config;
extern MIMEController	mime;

class responseMsg : public HTTPMsg {
	protected:
		double		http_version;
		int			status_code;
		std::string	reason;
		std::string	extension;
	public:
		void				resetMessage(void) {
			start_line = "";
			header_field.clear();
			msg_body = "";
			http_version = 0;
			status_code = 0;
			reason = "";
			extension = "";
		}

		void				setHttpVersion(double version) {
			this->http_version = version;
		}

		void				setStatusCode(int code) {
			this->status_code = code;
		}

		void				setExtension(std::string _extension) {
			this->extension = _extension;
		}

		void				setReason(void) {
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
		void				setResponseHeaderField(void) {
			header_field.insert(std::make_pair<std::string, std::string>("Content-Type", mime.getMIME(extension)));
			setHeaderField("Content-Length", std::to_string(this->getMsgBody().length()));
			header_field.insert(std::make_pair<std::string, std::string>("Accept-Ranges", "bytes"));
		}

		// HTTP/1.1 200 OK
		void				setStartLine(int status_code, double http_version) {
			this->setHttpVersion(http_version);
			this->setStatusCode(status_code);
			this->setReason();

			start_line += "HTTP/";
			start_line += std::to_string(http_version);
			start_line += " ";
			start_line += std::to_string(status_code);
			start_line += " ";
			start_line += reason;
		}

		std::string			makeResponseMsg(void) {
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
		void				setMsgBody(std::string uri) {
			std::cout << "URI : " << uri << std::endl;
			if (extension.compare("jpg") == 0 || extension.compare("ico") == 0) {
				std::cout << "IMAGE" << std::endl;
				std::string temp;
				int pos = uri.find_last_of(".");
				temp =	uri.substr(pos - 5);
				std::ifstream is(uri, std::ifstream::binary);
				if (is) {
					is.seekg(0, is.end);
					int	length = is.tellg();
					is.seekg(0, is.beg);

					char	*buffer = NULL;

					is.read((char *)buffer, length);
					std::string	tmp(buffer);
					msg_body = tmp;
					is.close();
				}
			} else {
				std::ifstream	file(uri);
				std::string		line;
				int				i = 0;

				while (std::getline(file, line)) {
					msg_body += line;
					msg_body += "\n";
					i++;
					if (i == 5)
						std::cout << "BODY : \n" << msg_body << std::endl;
				}
			}
		}

		int					isDirOrFile(requestMsg *reqMsg, std::string *uri) {
			int				flag = 0;
			DIR				*dp;
			struct dirent	*dir;

			*uri = config.getConfig("root") + reqMsg->getUriDir();

			if ((dp = opendir(uri->c_str())) == NULL) {
				std::cout << "opendir error" << std::endl;
				*uri = config.getConfig("root") + "/403.html";
				return (403);
			}
			if (reqMsg->getUriDir().compare("/") != 0)
				*uri += "/";
			*uri += reqMsg->getUriFile();
			while ((dir = readdir(dp)) != NULL) {
				if (dir->d_ino == 0)
					continue ;
				if (strcmp(reqMsg->getUriFile().c_str(), dir->d_name) == 0) {
					std::string _ext;
					int pos = reqMsg->getUriFile().find_last_of(".");
					_ext = reqMsg->getUriFile().substr(pos + 1);
					this->setExtension(_ext);
					flag = 200;
					break ;
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

		static std::string	setResponseMsg(requestMsg *reqMsg) {
			responseMsg		resMsg;
			std::string		uri;
			int				flag;

			flag = resMsg.isDirOrFile(reqMsg, &uri);
			resMsg.setStartLine(flag, reqMsg->getHttpVersion());
			resMsg.setMsgBody(uri);
			resMsg.setResponseHeaderField();
			std::string rtn = resMsg.makeResponseMsg();
			return (rtn);
		}
};

#endif
