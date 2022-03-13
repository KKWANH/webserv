#include "SocketController.hpp"

#include <stdlib.h>
#include <iostream>
#include "ConfigBlocks.hpp"
#include "ConfigController.hpp"

extern NginxConfig::GlobalConfig _config;

SocketController::SocketController() {};

SocketController::SocketController(int port) {
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
};

SocketController::~SocketController() {
	close(serverSocket);
};

int		SocketController::getServerBlockNum(){
	int port = int(ntohs(serverAddress.sin_port));
	for (int i = 0; i < (int)_config._http._server.size(); i++) {
		if (atoi(_config._http._server[i]._dir_map["listen"].c_str()) == port)
			return (i);
	}
	// TODO
	// ErrorHandler
	return (-1);
}


void	SocketController::generator(int port) {
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port); 
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
};

int		SocketController::binding() {
	int e1 = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	int e2 = listen(serverSocket, 42);
	int e3 = fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if (e1 < 0 || e2 < 0 || e3 < 0) {
		std::cout << "바인드 에러~" << std::endl;
		exit(1);
	}
	return serverSocket;
};

int		SocketController::run() {
	// clientAddress를 이용해 요청한 클라이언트의 IP와 포트를 알 수 있음.
	return accept(serverSocket, (struct sockaddr *) &clientAddress, &addressSize);
};