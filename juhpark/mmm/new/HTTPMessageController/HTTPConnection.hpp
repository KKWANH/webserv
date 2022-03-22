#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "ClassController.hpp"
#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"
#include "KernelQueueController.hpp"
#include "FileController.hpp"
#include <fcntl.h>

#define BUF_SIZ 2048

extern NginxConfig::GlobalConfig _config;

class HTTPConnection : public ClassController {
	public:
		typedef enum		e_Seq {
			REQUEST,
			REQUEST_TO_RESPONSE,
			RESPONSE,
			READY_TO_CGI,
			CGI_READ,
			CGI_WRITE,
			READY_TO_FILE,
			FILE_READ,
			FILE_WRITE,
			CLOSE,
			RE_KEEPALIVE
		}					Seq;

	private:
		Seq					seq;
		int					socket_fd;
		int					file_fd;
		int					cgi_fd;
		char				buffer[BUF_SIZ];
		HTTPData*			http_data;
		RequestMessage*		request_message;
		ResponseMessage*	response_message;
		CGIProcess*			cgi_process;
		int					readLength;
		int					writeLength;
		bool				keep_alive;
		
	public:
		HTTPConnection(int fd, int block, int server_port, std::string client_ip);
		virtual ~HTTPConnection();

		int	getServerBlock(void);
		int	getFileFd(void);
		int	getSocketFd(void);
		int	getCgiFd(void);
		
		int run();
};

#endif
