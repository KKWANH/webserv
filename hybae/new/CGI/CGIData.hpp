#ifndef CGIDATA_HPP
# define CGIDATA_HPP

#include <iostream>
#include <map>
#include "HTTPData.hpp"

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

            this->argv[0] = new char[this->data->CGI_root.size() + 1];
            strcpy(argv[0], c_str(this->data->CGI_root));
            
			this->argv[1] = new char[25];
            strcpy(argv[1], "./../../test/sample.php");
            
            this->argv[2] = new char[data->query_string.size() + 1];
            strcpy(argv[2], c_str(data->query_string));

            argv[3] = NULL;
        }
};

#endif
