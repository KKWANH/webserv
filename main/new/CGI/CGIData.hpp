#ifndef CGIDATA_HPP
# define CGIDATA_HPP

#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <iostream>
#include "../ErrorHandler/ErrorHandler.hpp"

class CGIData {
	private:
        char** 		argv;
		char**		envp;
        int			inputPair[2];
        int 		outputPair[2];
		int			env_size;
		std::string	only_root;
		std::string	only_file;
        pid_t 		_pid;
        std::string root;
        std::string query_string;
	public:
		CGIData() : root("Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"), query_string("123=abc") 
		{
			setCGIArgv();
			setEnvp();
            for (int i = 0; i < 3; i++)
                std::cout << "argv[" << i << "] :" << argv[i] << std::endl;
            for (int i = 0; i < env_size; i++)
                std::cout << envp[i] << std::endl;
		}

		~CGIData()
		{
			
			int status;
			if (_pid < 0) {
				waitpid(_pid, &status, WNOHANG);
				if (status & 0177) {
					kill(_pid, SIGTERM);
				}
			}
			//if (close(inputPair) == -1 || close(outputPair) == -1)
			//	throw ErrorHandler(__FILE__, __func__, __LINE__, "CGI pipes close mat gam");
			
			for (int i = 0; i < 3; i++)
				delete argv[i];
			delete argv;

			for (int i = 0; i < env_size; i++)
				delete envp[i];
			delete envp;
		}


        // execve 1번째 파라미터
        char*   getFilePath() { return (argv[0]); }

        // execve 2번째 파라미터
        char**  getArgv() { return (argv); }

        // execve 3번째 파라미터, setEnvp의 후속으로
        char    **generateEnvp(std::map<std::string, std::string> env)
        {
            char    **ret = new char*[env.size() + 1];
            int i = 0;
            std::map<std::string, std::string>::iterator it;
			env_size = env.size();
            for (it = env.begin(); it != env.end(); ++it)
            {
                ret[i] = new char[(it->first.length() + it->second.length() + 2)];
                strcpy(ret[i], it->first.c_str());
                strcat(ret[i], "=");
                strcat(ret[i], it->second.c_str());
                i++;
            }
            ret[i] = NULL;
            i = env_size;
            return (ret);
        }

        void    setCGIArgv() {
            this->argv = new char*[4];

            this->argv[0] = new char[root.size() + 1];
            strcpy(argv[0], root.c_str());

			this->argv[1] = new char[27];
            strcpy(argv[1], "./../cgiBinary/sample.php");

            this->argv[2] = new char[query_string.size() + 1];
            strcpy(argv[2], query_string.c_str());
            argv[3] = NULL;
        }

		void setEnvp() {
			//RequestMessage *reqMsg = static_cast<RequestMessage*>(requestMessage);
			std::map<std::string, std::string> _envMap;
			//cgi루트에서 파일명만 뺴고 싶다면
			size_t start = root.find_last_of("/");
			size_t finish = root.find_last_of(root);
			only_file = root.substr(start + 1, finish - start);
			only_root = root.substr(0, start);
			//일단루트랑 파일명이랑 분리할 필요가 있음

			// local 환경변수에 이미 존재하는 아이들
			_envMap[std::string("USER")] = std::string(std::getenv("USER"));
			_envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
			_envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
			_envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
			// parsing으로 가져온 아이들
			_envMap[std::string("REQUEST_METHOD")] = "GET";
			_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
			_envMap[std::string("REQUEST_SCHEME")] = "GET";
			_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
			_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

			_envMap[std::string("CONTENT_TYPE")] = "php-cgi";
			_envMap[std::string("CONTENT_LENGTH")] = "30";
//			정말 헤더의 저것을 나타내는게 맞나?

			_envMap[std::string("REMOTE_ADDR")] = "127.0.0.1"; // server socket addr
			_envMap[std::string("SERVER_PORT")] = "4242"; // host port
			// 요부분은 RFC에 명시되어 있어서 해둬야 할듯
			_envMap[std::string("QUERY_STRING")] = query_string;
			_envMap[std::string("SCRIPT_NAME")] = root;
			_envMap[std::string("PATH_INFO")] = root;
//			cgi스크립트가 어디에 있는거지
//			저거 맞겠지?

			//여기는 binary가 있는 루트경로 + binary파일 이름
			//그럼 여걸 어캐 어디서 파싱해서 넣지
			// _envMap[std::string("REQUEST_URI")] = requestMessage->_originURI;
			_envMap[std::string("DOCUMENT_ROOT")] = only_root;
			_envMap[std::string("REQUEST_URI")] = root;
			_envMap[std::string("DOCUMENT_URI")] = root;
			// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
			_envMap[std::string("SCRIPT_FILENAME")] = root;
			//그 이외RFC에 있는데 안들어간거
			//일단 공통적으로 joopark님 예시에 없던것들
			//AUTH_TYPE ->식별자
			//PATH_TRANSLATED -> 로컬 변환값, 대충 cgi안에 있는 경로 불면 될듯한데...
			//REMOTE_IDENT ->얜 안넣어도 된다고
			//REMOTE_USER ->식별자2
			//SERVER_NAME -> 얜 conf에 있는거 돚거하면 될듯
			//SERVER_SOFTWARE

			envp = generateEnvp(_envMap);

			//환경변수가 잘들갔나
	//		for (std::map<std::string, std::string>::iterator iter = _envMap.begin(); iter != _envMap.end(); iter++) {
	//			std::cout << iter->first << " = " << iter->second << std::endl;
	//		}
		}

		int& getInputPair(void)
		{
			return (inputPair[1]);
		}

		int& getOutputPair(void)
		{
			return (outputPair[0]);
		}

		//얜 타겟 없어도 되는건가
		void run(void)
		{
            std::cout << "RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
			if (pipe(this->inputPair) == -1 || pipe(this->outputPair) == -1) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Pipe Making Error.");
			}
			//fork
			if ((_pid = fork()) < 0) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Process Making Error.");
			}
			//자식
			if (_pid == 0) {
                 std::cout << "RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
				if ((dup2(this->inputPair[0], STDIN_FILENO) == -1) || (dup2(this->outputPair[1], STDOUT_FILENO) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "duplicate File Descriptor Error.");
				}
                std::cout << "RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
				if ((close(this->inputPair[1]) == -1) || (close(this->outputPair[0]) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error1.");
				} 
				std::cout << "arg : " << argv[0] << "env : " << envp[0] << std::endl;
				if (execve(this->argv[0], argv, envp) == -1) {
                    std::cout << "error" << std::endl;
					//throw ErrorHandler(__FILE__, __func__, __LINE__, "execve File Descriptor Error.");
                    return ;
				}
			} else {
				if ((close(this->inputPair[0]) == -1) || (close(this->outputPair[1]) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error2.");
				}
			}
		}


};

#endif