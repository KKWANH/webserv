#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <iostream>

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
		CGIData() : root("Users/mac/Documents/websv/webserv/juhpark/mmm/new/cgiBinary/php-cgi"), query_string("123=abc") 
		{
			setCGIArgv();
			setEnvp();
            for (int i = 0; i < 3; i++)
                std::cout << "argv[" << i << "] :" << argv[i] << std::endl;
			//환경변수가 잘들갔나
            for (int i = 0; i < env_size; i++)
                std::cout << envp[i] << std::endl;
		}

		~CGIData()
		{

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

  //          this->argv[0] = new char[root.size()];
    //        strcpy(argv[0], root.c_str());
            this->argv[0] = new char[22];
            strcpy(argv[0], "./../cgiBinary/php-cgi");

			this->argv[1] = new char[25];
            strcpy(argv[1], "./../cgiBinary/sample.php");

            this->argv[2] = new char[query_string.size()];
            strcpy(argv[2], query_string.c_str());
            argv[3] = NULL;
        }

		void setEnvp() {
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
	//		_envMap[std::string("REQUEST_METHOD")] = "GET";
			_envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
			_envMap[std::string("REQUEST_SCHEME")] = "GET";
			_envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
	//		_envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

			_envMap[std::string("CONTENT_TYPE")] = "php-cgi";
			_envMap[std::string("CONTENT_LENGTH")] = "22";

			_envMap[std::string("REMOTE_ADDR")] = "127.0.0.1"; // server socket addr
			_envMap[std::string("SERVER_PORT")] = "4242"; // host port
			_envMap[std::string("QUERY_STRING")] = query_string;
			_envMap[std::string("SCRIPT_NAME")] = root;
			_envMap[std::string("PATH_INFO")] = root;

			_envMap[std::string("DOCUMENT_ROOT")] = only_root;
			_envMap[std::string("REQUEST_URI")] = root;
			_envMap[std::string("DOCUMENT_URI")] = root;
			_envMap[std::string("SCRIPT_FILENAME")] = root;
			//그 이외RFC에 있는데 안들어간거
			//AUTH_TYPE ->식별자
			//PATH_TRANSLATED -> 로컬 변환값, 대충 cgi안에 있는 경로 불면 될듯한데...
			//REMOTE_IDENT ->얜 안넣어도 된다고
			//REMOTE_USER ->식별자2
			//SERVER_NAME -> 얜 conf에 있는거 돚거하면 될듯
			//SERVER_SOFTWARE

			envp = generateEnvp(_envMap);

		}

		//얜 타겟 없어도 되는건가
		void run(void)
		{
            std::cout << "RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
			if (pipe(this->inputPair) == -1 || pipe(this->outputPair) == -1) {
				std::cout << "Pipe Making Error."<<std::endl;
				return ;
			}
			//fork
			if ((_pid = fork()) < 0) {
				std::cout << "Process Making Error." << std::endl;
			}
			//자식
			if (_pid == 0) {
                 std::cout << "child RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
				if ((dup2(this->inputPair[0], STDIN_FILENO) == -1) || (dup2(this->outputPair[1], STDOUT_FILENO) == -1)) {
					std::cout << "duplicate File Descriptor Error." << std::endl;
					exit(1);
				}
//				std::cerr << "222RUNRURNRURFHSDFUEWFEWFSDFSDfsd" << std::endl;
				if ((close(this->inputPair[1]) == -1) || (close(this->outputPair[0]) == -1)) {
					std::cout << "File Descriptor closing Error1." << std::endl;;
					exit(1);
				} 
//				std::cerr << "arg : " << argv[0] << " env : " << envp[0] << std::endl;
				if (execve(this->argv[0], argv, envp) == -1) {
                   	std::cout << "execve error" << std::endl;
					//throw ErrorHandler(__FILE__, __func__, __LINE__, "execve File Descriptor Error.");
                    exit(1);
				}
			} else if (_pid > 0) {
				if ((close(this->inputPair[0]) == -1) || (close(this->outputPair[1]) == -1)) {
					std::cout << "File Descriptor closing Error2." << std::endl;
					return ;
				}
				int status;
				waitpid(_pid, &status, WNOHANG);
				char buf[1024];
				int len = 0;
				while (len == 0)
				{
					len = read(this->outputPair[0], buf, 1024);
					if (len < 0) {
						std::cout << "read error" << std::endl;
						return ;
					}
					buf[len] = '\0';
				}
				std::cout << " buf : " << buf << std::endl;
			}
		}


};

int main()
{
	CGIData cgi;
	cgi.run();
}
