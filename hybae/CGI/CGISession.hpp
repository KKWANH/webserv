
#ifndef CGISESSION_H
# define CGISESSION_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <unistd.h>
#include <signal.h>
#include "../ErrorHandler/ErrorHandler.hpp"
#include "../HTTPMessageController/HTTPMessageController.hpp"

class CGIProcess {
    private:
        char** _env;
        char* _arg[4];
        pid_t _pid;
        int _inputPair[2];
        int _outputPair[2];
        char    **generateEnvp(std::map<std::string, std::string> env)
        {
            char **envp = (char **)malloc(sizeof(char *) * (env.size() + 1));
            int i = 0;
            std::map<std::string, std::string>::iterator it;
            for (it = env.begin(); it != env.end(); ++it)
            {
                envp[i] = (char *)malloc(sizeof(char) * (it->first.length() + it->second.length() + 2));
                strcpy(envp[i], it->first.c_str());
                strcat(envp[i], "=");
                strcat(envp[i], it->second.c_str());
                i++;
            }
            envp[i] = NULL;
            return envp;
        }
    public:
        void setEnvp(RequestMessage* requestMessage)
        {
            std::map<std::string, std::string> _envMap;
            _envMap[std::string("USER")] = std::string(std::getenv("USER"));
            _envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
            _envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
            _envMap[std::string("PWD")] = std::string(std::getenv("PWD"));
            // ex) localhost:9090/data/index.html?id=123
            // _envMap[std::string("QUERY_STRING")] = requestMessage->_URIQueryString; // id=123
            _envMap[std::string("REQUEST_METHOD")] = requestMessage->getMethod(); // GET
            // _envMap[std::string("CONTENT_TYPE")] = requestMessage->_reqContentType;
            // _envMap[std::string("CONTENT_LENGTH")] = requestMessage->_reqContentLength;
            // _envMap[std::string("SCRIPT_NAME")] = requestMessage->_URIFilePath; // /data/index.html
            _envMap[std::string("PATH_INFO")] = requestMessage->getUriDir(); // /data/index.html
            // _envMap[std::string("REQUEST_URI")] = requestMessage->_originURI;
            // _envMap[std::string("DOCUMENT_URI")] = requestMessage->_reqURI; // /data/index.html?id=123
            // _envMap[std::string("DOCUMENT_ROOT")] = requestMessage->_root; // root directory
            _envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
            _envMap[std::string("REQUEST_SCHEME")] = requestMessage->getMethod(); // GET
            _envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
            _envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");
            // _envMap[std::string("REMOTE_ADDR")] = requestMessage->_clientIP; // server socket addr
            // _envMap[std::string("REMOTE_PORT")] = requestMessage->_clientPort; // client port
            // _envMap[std::string("SERVER_ADDR")] = requestMessage->_hostIP; // client socket addr
            // _envMap[std::string("SERVER_PORT")] = requestMessage->_hostPort; // host port
            // _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
            // _envMap[std::string("SCRIPT_FILENAME")] = requestMessage->_resAbsoluteFilePath;
            // for (std::map<std::string, std::string>::iterator iter = requestMessage->_HTTPCGIENV.begin(); iter != requestMessage->_HTTPCGIENV.end(); iter++) {
            //     _envMap[iter->first] = iter->second;
            // }
            // _arg[0] = const_cast<char*>(requestMessage->_CGIBinary.c_str());
            // _arg[1] = const_cast<char*>(requestMessage->_resAbsoluteFilePath.c_str());
            // if (requestMessage->_URIQueryString.empty()) {
            //     _arg[2] = NULL;
            // } else {
            //     _arg[2] = const_cast<char*>(requestMessage->_URIQueryString.c_str());
            // }
            // _arg[3] = NULL;
            _env = generateEnvp(_envMap);
        }

        ~CGIProcess()
        {
            int status;
            if (_pid > 0) {
                waitpid(_pid, &status, WNOHANG);
                if (status & 0177) {
                    kill(_pid, SIGTERM);
                }
            }
            if (getInputPair() > 0 && close(getInputPair()) == -1) {
                throw ErrorHandler("File Descriptor closing Error.");
            }
            if (getOutputPair() > 0 && close(getOutputPair()) == -1) {
                throw ErrorHandler("File Descriptor closing Error.");
            }
            if (_env != NULL) {
                int i = 0;
                while (_env[i] != NULL) {
                    delete _env[i];
                    i++;
                }
                delete _env;
            }
        }

        int& getInputPair(void)
        {
            return (_inputPair[1]);
        }

        int& getOutputPair(void)
		{
            return (_outputPair[0]);
        }

        void CGIprocess(int inputFd)
        {
            int target;

            if (pipe(this->_inputPair) == -1 || pipe(this->_outputPair) == -1) {
                throw ErrorHandler("Pipe Making Error.");
            }
            target = (inputFd != 0) ? inputFd : this->_inputPair[0];
            if ((_pid = fork()) < 0) {
                throw ErrorHandler("Process Making Error.");
            }
            if (_pid == 0) {
                if ((dup2(target, STDIN_FILENO) == -1) || (dup2(this->_outputPair[1], STDOUT_FILENO) == -1)) {
                    throw ErrorHandler("duplicate File Descriptor Error.");
                }
                if ((close(this->_inputPair[1]) == -1) || (close(this->_outputPair[0]) == -1)) {
                    throw ErrorHandler("File Descriptor closing Error.");
                }
                if (execve(this->_arg[0], _arg, _env) == -1) {
                    throw ErrorHandler("duplicate File Descriptor Error.");
                }
            } else {
                if ((close(this->_inputPair[0]) == -1) || (close(this->_outputPair[1]) == -1)) {
                    throw ErrorHandler("File Descriptor closing Error.");
                }
            }
            if (inputFd == 0) {
                close(this->_inputPair[1]);
                this->_inputPair[1] = 0;
            }
        }
};

#endif

