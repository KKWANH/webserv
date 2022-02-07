#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <exception>
#include <ctime>
#include <string>
#include <sys/errno.h>

class ErrorHandler : public std::exception {
    public:
        typedef enum e_ErrCode {NORMAL, ALERT, CRITICAL} ErrCode;
    private:
        std::string _errmsg;
        std::string _at;
        ErrorHandler::ErrCode _errcode;
    public:
        ErrorHandler();
        ErrorHandler(std::string errmsg, ErrorHandler::ErrCode errcode);
        ErrorHandler(std::string errmsg, ErrorHandler::ErrCode errcode, std::string at);
        virtual ~ErrorHandler() throw();
        ErrorHandler::ErrCode getErrorcode(void) const;
        static char* getTime(void);
        virtual const char* what() const throw();
};


ErrorHandler::ErrorHandler() : _errmsg(NULL), _at(NULL), _errcode(NORMAL) {}

ErrorHandler::ErrorHandler(std::string errmsg, ErrorHandler::ErrCode errcode) : _errmsg(errmsg), _at(NULL), _errcode(errcode) {}

ErrorHandler::ErrorHandler(std::string errmsg, ErrorHandler::ErrCode errcode, std::string at) : _errmsg(errmsg), _at(at), _errcode(errcode) {};

ErrorHandler::~ErrorHandler() throw() {}

ErrorHandler::ErrCode ErrorHandler::getErrorcode(void) const {
    return (_errcode);
}

char * ErrorHandler::getTime(void) {
    static char timeBuffer[20];
    time_t rawtime;
    struct tm *timeinfo;

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(timeBuffer, 20, "%Y/%m/%d %H:%M:%S", timeinfo);
    return (timeBuffer);
}

const char* ErrorHandler::what() const throw() {
    static std::string rtn;

    rtn.clear();
    rtn.append(getTime());
    if (_errcode == NORMAL) {
        rtn.append("\033[0;32m [normal] ");
    } else if (_errcode == ALERT) {
        rtn.append("\033[0;33m [alert] ");
    } else if (_errcode == CRITICAL) {
        rtn.append("\033[0;31m [critical] ");
    }
    rtn.append(_errmsg);
    if (errno != 0) {
        rtn.append(" (");
        rtn.append(std::strerror(errno));
        rtn.append(")");
        errno = 0;
    }
    if (_at.empty() == false) {
        rtn.append(" at ");
        rtn.append(_at);
    }
    rtn.append("\033[0m");
    return (rtn.c_str());
}

#endif