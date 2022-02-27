#ifndef CGIDATA_HPP
# define CGIDATA_HPP
#include <iostream>
// /
// #include "./../HTTPMessageController/RequestMessageController.hpp"
// #include "./../HTTPMessageController/ResponseMessageController.hpp"

// extern ConfigController config;
// extern MIMEController mime;

class CGIData {
	private:
		std::map<std::string, std::string> env;
        char** argv;
	public:

        // execve 1번째 파라미터
        char*   getFilePath() { return (argv[0]); }

        // execve 2번째 파라미터
        char**  getArgv() { return (argv); }

        // execve 3번째 파라미터
        char    **generateEnvp(std::map<std::string, std::string> env)
        {
            char    **envp = new char*[env.size() + 1];
            int i = 0;
            std::map<std::string, std::string>::iterator it;
            for (it = env.begin(); it != env.end(); ++it)
            {
                envp[i] = new char[(it->first.length() + it->second.length() + 2)];
                strcpy(envp[i], it->first.c_str());
                strcat(envp[i], "=");
                strcat(envp[i], it->second.c_str());
                i++;
            }
            envp[i] = NULL;
            return envp;
        }

        void    setCGIArgv() {
            this->argv = new char*[4];

            this->argv[0] = new char[22];
            strcpy(argv[0], "./../../test/php-cgi");  // cgi file 경로. config 파일에서 파싱해서 사용
            
			this->argv[1] = new char[25];
            strcpy(argv[1], "./../../test/sample.php");  // 실행할 파일 경로. request message의 uri에서 파싱해서 사용
            
            this->argv[2] = new char[10];
            strcpy(argv[2], "var=1234");
            /*
            querystring 값
            if (requestMessage->_URIQueryString.empty())
                argv[2] = NULL;
            else
                argv[2] = const_cast<char*>(requestMessage->_URIQueryString.c_str());
            */
            argv[3] = NULL;
        }
};

#endif