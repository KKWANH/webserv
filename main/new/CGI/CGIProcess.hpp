#ifndef CGIPROCESS_HPP
#define CGIPROCESS_HPP

#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <map>
#include "HTTPData.hpp"
#include "ConfigBlocks.hpp"
extern NginxConfig::GlobalConfig _config;

class CGIProcess {
	private:
		char**		envp;
        char** 		argv;
        int			inputPair[2];
        int 		outputPair[2];
		int			env_size;
		std::string	only_root;
		std::string	only_file;
        pid_t 		_pid;
	public:
		CGIProcess(HTTPData* data)
		{
			setCGIArgv(data);
			setEnvp(data);
			
			#if 0
            for (int i = 0; i < 3; i++)
                std::cout << "argv[" << i << "] :" << argv[i] << std::endl;
            for (int i = 0; i < env_size; i++)
                std::cout << envp[i] << std::endl;
			#endif
		}

		~CGIProcess()
		{
			int status;
			if (_pid > 0) {
				waitpid(_pid, &status, 0);
				if (status & 0177) {
					kill(_pid, SIGTERM);
				}
			}
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
            return (ret);
        }

        void    setCGIArgv(HTTPData* data) {
            this->argv = new char*[4];

            this->argv[0] = new char[data->CGI_root.size() + 1];
            strcpy(argv[0], data->CGI_root.c_str());

			this->argv[1] = new char[59];
			// TODO
			// 요청으로 들어온 경로를 절대경로로 넣어줄 것
            strcpy(argv[1], "/Users/hybae/Desktop/webserv/main/new/cgiBinary/sample.php");

            this->argv[2] = new char[data->query_string.size() + 1];
            strcpy(argv[2], data->query_string.c_str());

            argv[3] = NULL;
        }

		void setEnvp(HTTPData* data) {
			std::map<std::string, std::string> _envMap;
			//cgi루트에서 파일명만 뺴고 싶다면
			std::string root = data->CGI_root.substr(1, data->CGI_root.size());
			size_t start = root.find_last_of("/");
			size_t finish = root.find_last_of(root);
			only_file = root.substr(start + 1, finish - start);
			only_root = root.substr(0, start);
			
			_envMap[std::string("USER")] = std::string(std::getenv("USER"));
			_envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
			_envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
			_envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
			_envMap[std::string("REQUEST_METHOD")] = data->method;
			_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
			_envMap[std::string("REQUEST_SCHEME")] = data->method;
			_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
			_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");
			_envMap[std::string("CONTENT_TYPE")] = data->header_field["Content-Type"];
			_envMap[std::string("CONTENT_LENGTH")] = data->header_field["Content-Length"];
			_envMap[std::string("REMOTE_ADDR")] = data->client_ip; // server socket addr
			_envMap[std::string("SERVER_PORT")] = std::to_string(data->server_port); // host port
			_envMap[std::string("QUERY_STRING")] = data->query_string;
			_envMap[std::string("SCRIPT_NAME")] = root;
			_envMap[std::string("PATH_INFO")] = root;
			_envMap[std::string("DOCUMENT_ROOT")] = only_root;
			_envMap[std::string("REQUEST_URI")] = data->uri_dir + data->uri_file; // 리퀘스트에 명시된 전체 주소가 들어가야 함
			_envMap[std::string("DOCUMENT_URI")] = data->uri_dir + data->uri_file; // 리퀘스트에 명시된 전체 주소가 들어가야 함
			// _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
			// TODO
			// envp[1]과 동일
			_envMap[std::string("SCRIPT_FILENAME")] = "/Users/hybae/Desktop/webserv/main/new/cgiBinary/sample.php"; // 실행하고자 하는 파일의 절대 경로가 들어가야 함.

			envp = generateEnvp(_envMap);

			//환경변수가 잘들갔나
	//		for (std::map<std::string, std::string>::iterator iter = _envMap.begin(); iter != _envMap.end(); iter++) {
	//			std::cout << iter->first << " = " << iter->second << std::endl;
//			}
		}

		//파이프는 1로 들어가서 0으로 나온다
		/*
		 * 				파이프A[1] -----------------> 파이프A[0]
		 *		메인												자식
		 *				파이프B[0] <----------------- 파이프B[1]
		 * */
		int& getInputPair(void)
		{
			return (inputPair[1]);
		}

		int& getOutputPair(void)
		{
			return (outputPair[0]);
		}

		//얜 타겟 없어도 되는건가
		//그건 POST메소드에서 타겟이 있다면 해당 타겟을fd화 시켜서 인자로 넣으면 됨
		//킹치만 그건 없었으니...
		void run(void)
		{
			if (pipe(this->inputPair) == -1 || pipe(this->outputPair) == -1) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Pipe Making Error.");
			}
			//fork
			if ((_pid = fork()) < 0) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Process Making Error.");
			}
			//자식
			if (_pid == 0) {
				if ((dup2(this->inputPair[0], STDIN_FILENO) == -1) || (dup2(this->outputPair[1], STDOUT_FILENO) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "duplicate File Descriptor Error.");
				}
				if ((close(this->inputPair[1]) == -1) || (close(this->outputPair[0]) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error1.");
				}
				if (execve(this->argv[0], argv, envp) == -1) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "execve File Descriptor Error.");
				}
			} else {
				if ((close(this->inputPair[0]) == -1) || (close(this->outputPair[1]) == -1)) {
					throw ErrorHandler(__FILE__, __func__, __LINE__, "File Descriptor closing Error2.");
				}
			}
		}
};

#endif
