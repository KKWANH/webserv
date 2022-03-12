#ifndef SOCKETCONTROLLER_HPP
# define SOCKETCONTROLLER_HPP

#include "ClassController.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

class SocketController : public ClassController {
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t addressSize;
	int serverSocket;

public:
	SocketController();
	SocketController(int port);
	virtual ~SocketController();
	struct sockaddr_in getServerAddr();
	struct sockaddr_in getClientAddr();

	
	int 	getServerBlockNum();

	void	generator(int port);
	int		binding();
	int		run();
};

#endif
