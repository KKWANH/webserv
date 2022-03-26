// NOTE: modified coding convention by joopark
#ifndef SOCKETCONTROLLER_HPP
# define SOCKETCONTROLLER_HPP

#include "ClassController.hpp"
#include "ConfigBlocks.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "ErrorHandler.hpp"

class SocketController : public ClassController {
private:
	struct sockaddr_in		serverAddress;
	struct sockaddr_in		clientAddress;
	socklen_t				addressSize;
	int						serverSocket;

public:
	SocketController();
	SocketController(int port);
	virtual ~SocketController();
	
	int 	getServerBlockNum();

	struct sockaddr_in getServerAddr();
	struct sockaddr_in getClientAddr();

	void	generator(int port);
	int		binding();
	int		run();
};

#endif
