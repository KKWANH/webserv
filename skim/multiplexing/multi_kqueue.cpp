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

int		main(int ac, char *av[])
{
	int	ser_socket;
	int cli_socket;
	struct sockaddr_in	ser_addr;
	struct sockaddr_in	cli_addr;
	socklen_t			cli_addr_len;

	char		buf[1024];
	std::string httpTestHeaderString;
	int			read_len;

	// for kqueue
	int				kfd;
	struct timespec	pollingTime;
	struct kevent	eventSetting;
	struct kevent	getEvent[10];
	int				result;

	// polling time = 1 sec
	pollingTime.tv_sec = 1;
	pollingTime.tv_nsec = 0;

	// kernel event queue 생성
	kfd = kqueue();

	// HTTP Header 및 Body
	httpTestHeaderString += "HTTP/1.1 200 OK\r\n";
	httpTestHeaderString += "Content-Type: text/html\r\n";
	httpTestHeaderString += "\r\n";
	httpTestHeaderString += "<html>";
	httpTestHeaderString += "<head><title>Hello</title></head>";
	httpTestHeaderString += "<body>";
	httpTestHeaderString += "<b><center> I'm SKIM :) </center></b>";
	httpTestHeaderString += "</body>";
	httpTestHeaderString += "</html>";

	ser_socket = socket(PF_INET, SOCK_STREAM, 0); // 서버 소켓 생성
	if (ser_socket == -1)
	{
		std::cout << "socket() error" << std::strerror(errno) << std::endl;
		return (1);
	}

	// 127.0.0.1:[port] 서버 생성
	std::memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET; // ipv4
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip 주소
	ser_addr.sin_port = htons(atoi(av[1])); // port

	if (bind(ser_socket, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1)
	{
		std::cout << "bind() error" << std::strerror(errno) << std::endl;
		return (1);
	}

	// 연결 대기열에 42개를 생성해준다.
	if (listen(ser_socket, 42) == -1) {
		std::cout << "listen() error" << std::strerror(errno) << std::endl;
		return (1);
	}

	// fcntl 시스템 콜을 이용해 소켓 파일 디스크립터가 논블록킹으로 동작되게 한다.
	if (fcntl(ser_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "fcntl() error" << std::strerror(errno) << std::endl;
		return (1);
	}

	// accept를 하기 위한 serverSocket도 select로 감시함
	EV_SET(&eventSetting, ser_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
		std::cout << "kevent() error" << std::strerror(errno) << std::endl;
		return (1);
	}

	while (1) {
		result = kevent(kfd, NULL, 0, getEvent, 10, &pollingTime);
		if (result == -1) {
			std::cout << "kevent() error" << std::strerror(errno) << std::endl;
			return (1);
		} else if (result == 0) {
			std::cout << "waiting..." << std::endl;
		} else {
			for (int i = 0; i < result; i++) {
				if (getEvent[i].ident == ser_socket) {
					cli_socket = accept(ser_socket, (struct sockaddr *)&cli_addr, &cli_addr_len);
					if (cli_socket == -1) {
						std::cout << "accept() error" << std::strerror(errno) << std::endl;
						return (1);
					}
					std::cout << "fd INSERT" << std::endl;
					EV_SET(&eventSetting, cli_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
						std::cout << "kevent() error" << std::strerror(errno) << std::endl;
						return (1);
					}
				} else {
					read_len = read(getEvent[i].ident, buf, 1024);
					std::cout << "data : " << std::string(buf, read_len) <<  std::endl;
					write(getEvent[i].ident, httpTestHeaderString.data(), httpTestHeaderString.length());
					if (kevent(kfd, &eventSetting, 1, NULL, 0, NULL) == -1) {
						std::cout << "kevent() error" << std::strerror(errno) << std::endl;
						return (1);
					}
					close(getEvent[i].ident);
				}
			}
		}
	}
	close(ser_socket);
	return (0);
}
