
#ifndef CGISESSION_H
#define CGISESSION_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <unistd.h>
#include <signal.h>
#include "ErrorHandler.hpp"
// #include "RequestMessageController.hpp"

// class RequestMessage;

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

#endif

