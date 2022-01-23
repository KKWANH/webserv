#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "./HTTPMessageController.hpp"
#include <dirent.h>

extern ConfigController config;

class ServerProcess {
	public:
		// TODO: 권한으로 파일 탐색이 안 되는 경우에 대한 처리 추가 필요
		// request message에서 요청 된 uri에 index 파일이 있는지 확인
		// 파일이 있으면 0, 없으면 1, 해당 uri 자체가 없으면 -1 반환
		static	int			findIndexFile(int socket_client, std::string target, std::string* uri) {
			int						flag = 0;
			DIR						*dp;
			struct dirent	*dir;
			*uri = config.getConfig("root") + target;
			const char*		uri_char = uri->c_str();
			//std::cout << "URI : " << *uri << std::endl;

			if ((dp = opendir(uri_char)) == NULL) {
				//std::cout << "DIR Open Error" << std::endl;
				return (1);
			}

			while((dir = readdir(dp)) != NULL) {
				if (dir->d_ino == 0)
					continue;
				if (strcmp(config.getConfig("index").c_str(), dir->d_name) == 0) {
					flag = 1;
					break;
				}
			}
			closedir(dp);
			return (flag == 1 ? 0 : 1);
		}

		// server process
		static std::string	serverProcess(int socket_client, RequestMessage* requestMessage) {
			ResponseMessage 	responseMessage;
			std::string				uri;
			int								flag;

			flag = findIndexFile(socket_client, requestMessage->request_target, &uri);
			std::cout << "URI : " << uri << std::endl;
			if (flag == 0){
				responseMessage.setStatusCode(200);
				responseMessage.setMessageBody(uri + "/index.html");
				//std::cout << "index file location : " << uri << std::endl;
			}
			else if (flag == 1) {
				responseMessage.setStatusCode(404);
				responseMessage.setMessageBody(config.getConfig("root") + "/html/404.html");
				//std::cout << "there is no index file" << std::endl;
			}
			else {
				responseMessage.setStatusCode(403);
				responseMessage.setMessageBody(config.getConfig("root") + "/html/403.html");
				//std::cout << "incorrect location" << std::endl;
			}
			responseMessage.setHttpVersion(requestMessage->http_version);
			responseMessage.setReasonPhrase();
			responseMessage.setStartLine();
			responseMessage.setHeaderField();
			std::string rtn = responseMessage.makeResponseMessage();
			//std::cout << responseMessage.getStartLine() << std::endl;
			//std::cout << rtn << std::endl;
			return (rtn);
		}
};

#endif