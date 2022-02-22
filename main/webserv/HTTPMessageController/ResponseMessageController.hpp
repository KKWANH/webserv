#ifndef RESPONSEMESSAGECONTROLLER_HPP
# define RESPONSEMESSAGECONTROLLER_HPP

#include "./HTTPMessageController.hpp"
#include "./RequestMessageController.hpp"
#include "./../CGI/CGIProcess.hpp"

// Response Message class
// Request와 Response간 start-line이 달라서 각 클래스에서 따로 처리하기 위함.
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

            //요청 메시지에서 cgi(php)가 발견되었다면
            //read로 데이터를 읽어서 저장한다
			isCGI = true;
			if (isCGI) {
				CGIProcess	cgi;
				cgi.setEnvp();
				cgi.CGIprocess(0);
				std::cout << "cgi.getOutputPair() -> " << cgi.getOutputPair() << std::endl;
				char	buffer[1024];
				int n;
				while ((n = read(cgi.getOutputPair(), buffer, 1024)) > 0) {
					message_body += std::string(buffer);
					std::cout << "buf -> " << message_body << std::endl;
				}
                if (n < 0)
                    throw (ErrorHandler(__FILE__, __func__, __LINE__, "ERROR : READ CGI ERROR"));
				// cgi 에서 받은 메세지 리팩토링
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
			std::string			uri;
			int					flag;

			flag = responseMessage.isDirOrFile(requestMessage, &uri);

			responseMessage.setStartLine(flag, requestMessage->getHttpVersion());

			responseMessage.setMessageBody(uri);

			responseMessage.setResponseHeaderField();
			std::string rtn = responseMessage.makeResponseMessage();
			return (rtn);
		}
};

#endif
