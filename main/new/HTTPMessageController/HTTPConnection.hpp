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
			READY_TO_FILE,
			FILE_READ,
			FILE_WRITE,
			IS_CGI,
			CGI_WRITE,
			CLOSE,
			END
		}					Seq;

	private:
		Seq					seq;
		int					socket_fd;
		int					file_fd;
		char				buffer[BUF_SIZ];
		HTTPData*			http_data;
		RequestMessage*		request_message;
		ResponseMessage*	response_message;
		CGIProcess*			cgi_process;
		int					readLength;
		int					writeLength;
		
	public:
		HTTPConnection(int fd, int block, int server_port, std::string client_ip);
		virtual ~HTTPConnection();

		int	getServerBlock(void);
		int	getFileFd(void);
		int	getSocketFd(void);
		
		int run();
};

#endif