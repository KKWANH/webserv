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
	SocketController() {}
	SocketController(int port) {
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);
		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	}
	~SocketController() {
		close(serverSocket);
	}

	void	generator(int port) {
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);
		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	}

	int binding() {
		int e1 = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
		int e2 = listen(serverSocket, 42);
		int e3 = fcntl(serverSocket, F_SETFL, O_NONBLOCK);
		if (e1 < 0 || e2 < 0 || e3 < 0) {
			std::cout << "바인드 에러~" << std::endl;
			exit(1);
		}
		return serverSocket;
	}

	int run() {
		// clientAddress를 이용해 요청한 클라이언트의 IP와 포트를 알 수 있음.
		return accept(serverSocket, (struct sockaddr *) &clientAddress, &addressSize);
	}
};

#endif
