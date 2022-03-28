#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "ClassController.hpp"
#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"
#include "KernelQueueController.hpp"
#include "FileController.hpp"
#include "AutoindexController.hpp"
#include "ErrorPageController.hpp"
#include <fcntl.h>
#include <sstream>

#define BUF_SIZ 2048

extern NginxConfig::GlobalConfig _config;

class HTTPConnection : public ClassController {
	public:
		typedef enum				e_Seq {
			REQUEST, 				// 00
			READY_TO_MESSAGE_BODY,	// 01
			MESSAGE_BODY_READ,		// 02
			MESSAGE_BODY_WRITE,		// 03
			REQUEST_TO_RESPONSE,	// 04
			BODY_TO_RESPONSE,		// 05
			RESPONSE,				// 06
			READY_TO_CGI,			// 07
			CGI_READ,				// 08
			CGI_WRITE,				// 09
			READY_TO_FILE,			// 10
			FILE_READ,				// 11
			FILE_WRITE,				// 12
			CLOSE,					// 13
			RE_KEEPALIVE,			// 14
			AUTOINDEX_WRITE			// 15
		}							Seq;

	private:
		Seq							seq;
		int							socket_fd;
		int							file_fd;
		int							cgi_output_fd;
		int							cgi_input_fd;
		char						buffer[BUF_SIZ];
		HTTPData*					http_data;
		RequestMessage*				request_message;
		ResponseMessage*			response_message;
		CGIProcess*					cgi_process;
		ErrorPageController*		error_page_controller;
		int							readLength;
		int							writeLength;
		bool						keep_alive;
		int							current_size;	
		int							limit_size;

	public:
		HTTPConnection(
			int fd,
			int block,
			int server_port,
			std::string client_ip,
			std::string client_port,
			std::string host_ip,
			std::string host_port);

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
