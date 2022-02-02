#include <iostream>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

int main(void)
{
	// common
	int serverSocket;
	int clientSocket;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	char buffer[1024];
	std::string httpTestHeaderString;
	socklen_t addressSize;
	int readLength;

	// for kqueue (2)
	int kfd;
	struct timespec pollingTime;
	struct kevent eventSetting;
	struct kevent getEvent[10];
	int result;

	// HTTP Header 및 Body
	httpTestHeaderString += "HTTP/1.1 200 이 편지는 한국에서 시작되어 어쩌구저쩌구\r\n";
	httpTestHeaderString += "Content-Type: text/html\r\n";
	httpTestHeaderString += "\r\n";
	httpTestHeaderString += "<html>";
	httpTestHeaderString += "<head><title>hi</title></head>";
	httpTestHeaderString += "<body>";
	httpTestHeaderString += "<b><center> HI! </center></b>";
	httpTestHeaderString += "</body>";
	httpTestHeaderString += "</html>";

	pollingTime.tv_sec = 1;
	pollingTime.tv_nsec = 0;
	kfd = kqueue();

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cout << "err : " << std::strerror(errno) << std::endl;
		std::cout << "at : " << "socket (2)" << std::endl;
		return (1);
	}

	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(8080);

	if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
		std::cout << "err : " << std::strerror(errno) << std::endl;
		std::cout << "at : " << "bind (2)" << std::endl;
		return (1);
	}
	if (listen(serverSocket, 42) == -1) {
		std::cout << "err : " << std::strerror(errno) << std::endl;
		std::cout << "at : " << "listen (2)" << std::endl;
		return (1);
	}
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "err : " << std::strerror(errno) << std::endl;
		std::cout << "at : " << "fcntl (2)" << std::endl;
		return (1);
	}

	EV_SET(&eventSetting, serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
		std::cout << "err : " << std::strerror(errno) << std::endl;
		std::cout << "at : " << "kevent (2)" << std::endl;
		return (1);
	}

	while (true) {
		result = kevent(kfd, NULL, 0, getEvent, 10, &pollingTime);
		if (result == -1) {
			std::cout << "err : " << std::strerror(errno) << std::endl;
			std::cout << "at : " << "kevent (2)" << std::endl;
			return (1);
		} else if (result == 0) {
			std::cout << "waiting..." << std::endl;
		} else {
			for (int i = 0; i < result; i++) {
				if (getEvent[i].ident == serverSocket) {
					clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &addressSize);
					if (clientSocket == -1) {
						std::cout << "err : " << std::strerror(errno) << std::endl;
						std::cout << "at : " << "accept (2)" << std::endl;
						return (1);
					}
					std::cout << "fd INSERT" << std::endl;
					EV_SET(&eventSetting, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
						std::cout << "err : " << std::strerror(errno) << std::endl;
						std::cout << "at : " << "kevent (2)" << std::endl;
						return (1);
					}
				} else {
					readLength = read(getEvent[i].ident, buffer, 1024);
					std::cout << "data : " << std::string(buffer, readLength) << std::endl;
					write(getEvent[i].ident, httpTestHeaderString.data(), httpTestHeaderString.length());
					EV_SET(&eventSetting, getEvent[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					// if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
					// 	std::cout << "err : " << std::strerror(errno) << std::endl;
					// 	std::cout << "at : " << "kevent (2)" << std::endl;
					// 	return (1);
					// }
					close(getEvent[i].ident);
				}
			}
		}
	}
	close(serverSocket);
	return (0);
}