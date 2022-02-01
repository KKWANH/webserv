#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "./requestMsgController.hpp"
#include "./responseMsgController.hpp"
#include <dirent.h>

extern configController		config;

class serverProcess {
	public:
		static int			findIndexFile(int client_sock, std::string target, std::string *uri) {
			int				flag = 0;
			DIR				*dp;
			struct dirent	*dir;

			*uri = config.getConfig("root") + target;
			const char	*uri_char = uri->c_str();

			if ((dp = opendir(uri_char)) == NULL)
				return (1);

			while ((dir = readdir(dp)) != NULL) {
				if (dir->d_ino == 0)
					continue ;
				if (strcmp(config.getConfig("index").c_str(), dir->d_name) == 0) {
					flag = 1;
					break ;
				}
			}
			closedir(dp);
			return (flag == 1 ? 0 : 1);
		}

		static std::string	process(int client_sock, requestMsg *requestMsg) {
			responseMsg		responseMsg;
			std::string		uri;
			int				flag;

			flag = findIndexFile(client_sock, requestMsg->request_target, &uri);

			if (flag == 0) {
				responseMsg.setStatusCode(200);
				responseMsg.setMsgBody(uri + "/index.html");
			}
			else if (flag == 1) {
				responseMsg.setStatusCode(404);
				responseMsg.setMsgBody(config.getConfig("root") + "/html/404.html");
			}
			else {
				responseMsg.setStatusCode(403);
				responseMsg.setMsgBody(config.getConfig("root") + "/html/403.html");
			}
			responseMsg.setHttpVersion(requestMsg->http_version);
			responseMsg.setReason();
			responseMsg.setStartLine();
			responseMsg.setHeaderField();

			std::string rtn = responseMsg.makeResponseMsg();
			return (rtn);
		}
};

#endif
