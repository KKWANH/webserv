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
			READY_TO_MESSAGE_BODY,
			MESSAGE_BODY_READ,
			MESSAGE_BODY_WRITE,
			REQUEST_TO_RESPONSE,
			BODY_TO_RESPONSE,
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
		int					cgi_output_fd;
		int					cgi_input_fd;
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
		static void killConnection(void* hc);

		int	getServerBlock(void);
		int	getFileFd(void);
		int	getSocketFd(void);
		int	getCgiOutputFd(void);
		int	getCgiInputFd(void);
		
		int run();
};

#endif
