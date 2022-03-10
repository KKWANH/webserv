#ifndef CGIPROCESS_HPP
#define CGISESSION_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <map>
#include "HTTPData.hpp"

class CGIProcess {
	private:
		std::map<std::string, std::string> env;
        char** 		argv;
        int			 _inputPair[2];
        int 		_outputPair[2];
        pid_t 		_pid;
		HTTPData*	data;
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
            return (envp);
        }

        void    setCGIArgv() {
            this->argv = new char*[4];

            this->argv[0] = new char[this->data->CGI_root.size() + 1];
            strcpy(argv[0], c_str(this->data->CGI_root));
            
			this->argv[1] = new char[25];
            strcpy(argv[1], "./../../test/sample.php");
            
            this->argv[2] = new char[data->query_string.size() + 1];
            strcpy(argv[2], c_str(data->query_string));

            argv[3] = NULL;
        }

		void setEnvp() {
		RequestMessage *reqMsg = static_cast<RequestMessage*>(requestMessage);
		std::map<std::string, std::string> _envMap;

		// local 환경변수에 이미 존재하는 아이들
		_envMap[std::string("USER")] = std::string(std::getenv("USER"));
		_envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
		_envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
		_envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
		// parsing으로 가져온 아이들
		_envMap[std::string("REQUEST_METHOD")] = this->data->method;
		// _envMap[std::string("PATH_INFO")] = requestMessage->getUriDir(); // /data/index.html
		_envMap[std::string("PATH_INFO")] = std::string("/index.html");
		_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
		_envMap[std::string("REQUEST_SCHEME")] = this->data->method;
		_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
		_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

		_envMap[std::string("CONTENT_TYPE")] = mime.getMIME(this->data->CGI_what);
		//->여기경로는 어찌 잡아야 하지
		/*
		_envMap[std::string("CONTENT_LENGTH")] = reqMsg->getHeaderField("Content-Length");

		// _envMap[std::string("REMOTE_ADDR")] = requestMessage->_clientIP; // server socket addr
		// _envMap[std::string("REMOTE_PORT")] = requestMessage->_clientPort; // client port
		// _envMap[std::string("SERVER_ADDR")] = requestMessage->_hostIP; // client socket addr
		// _envMap[std::string("SERVER_PORT")] = requestMessage->_hostPort; // host port
		_envMap[std::string("QUERY_STRING")] = reqMsg->getQueryString();
		_envMap[std::string("SCRIPT_NAME")] = reqMsg->getUriFile();

		//여기는 binary가 있는 루트경로 + binary파일 이름
		//그럼 여걸 어캐 어디서 파싱해서 넣지
		// _envMap[std::string("REQUEST_URI")] = requestMessage->_originURI;
		_envMap[std::string("DOCUMENT_ROOT")] = "./cgiBinary";
		_envMap[std::string("REQUEST_URI")] = "./cgiBinary/" + reqMsg->getUriFile();
		_envMap[std::string("DOCUMENT_URI")] = "./cgiBinary/" + reqMsg->getUriFile();
		// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
		_envMap[std::string("SCRIPT_FILENAME")] = "./cgiBinary/" + reqMsg->getUriFile();

			// for (std::map<std::string, std::string>::iterator iter = requestMessage->_HTTPCGIENV.begin(); iter != requestMessage->_HTTPCGIENV.end(); iter++) {
			//	 _envMap[iter->first] = iter->second;
			// } // CGI binary 파일의 헤더 (HTTP_)
			// request의 헤더가 cgi 의 환경변수로 들어가야 한다.
		*/
		_env = generateEnvp(_envMap);

		//환경변수가 잘들갔나
		for (std::map<std::string, std::string>::iterator iter = _envMap.begin(); iter != _envMap.end(); iter++) {
			std::cout << iter->first << " = " << iter->second << std::endl;
			}
		}


};
/*
class CGIProcess {
    private:
        char** _env;
        char** _arg;
        pid_t _pid;
        int _inputPair[2];
        int _outputPair[2];

        char    **generateEnvp(std::map<std::string, std::string> env);
        void    setCGIarg();
    public:
        void setEnvp(void *requestMessage);

        ~CGIProcess();

        int& getInputPair(void);
        int& getOutputPair(void);
        void run(void);
};
*/
#endif

