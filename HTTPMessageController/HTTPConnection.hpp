#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"
#include "ClassController.hpp"
#include "ErrorPageController.hpp"
#include "FileController.hpp"
#include "AutoindexController.hpp"
#include "ErrorPageController.hpp"
#include "SocketController.hpp"


#include <fcntl.h>
#include <sstream>

#define BUF_SIZ 2048

extern NginxConfig::GlobalConfig _config;



class HTTPConnection : public ClassController {
    public:
	    typedef enum		e_Seq {
			HTTP_READ,
			READY_CGI_INPUT,
			READY_AUTOINDEX,
			READY_REDIRECT,
			READY_ERROR,
            CGI_INPUT_READ,
            CGI_INPUT_WRITE,
            FINISH_CGI_INPUT,
            CGI_GET_HEADER,
            SET_RESPONSE,
            WRITE_RESPONSE,
            CGI_OUTPUT_READ,
            CGI_OUTPUT_WRITE,
			BUFFER_WRITE,
            FILE_READ,
            FILE_WRITE,
            CLOSE
		}					Seq;
    private:
        Seq                 seq;

        HTTPData*           data;
        RequestMessage*     request;
        ResponseMessage*    response;
        CGIProcess*         cgi;



        int                 socket_fd;
        int                 file_fd;
        int                 read_len;
        int                 write_len;
        char                buffer[BUF_SIZ];
        bool                keep_alive;

    public:
        HTTPConnection(int fd, int block, SocketController* socket);
        virtual ~HTTPConnection();
        static void killConnection(void* hc);

        int run();

};

#endif