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
#include <vector>

void	change_event(std::vector<struct kevent> &eventList, uintptr_t ident, int16_t filter, uint16_t flags)
{
	struct kevent	tmp_event;

	EV_SET(&tmp_event, ident, filter, flags, 0, 0, NULL);
	eventList.push_back(tmp_event);
}

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


	// 변경사항
	// 1) kevent는 한번만 쓸 것 => eventSetting 대신 setEvent 사용
	// 2) read, write를 분리할 것 => filter의 속성을 이용해서 처리할 것

	std::vector<struct kevent> setEvent;

	// accept를 하기 위한 serverSocket도 select로 감시함
	change_event(setEvent, ser_socket, EVFILT_READ, EV_ADD);

	while (1) {
		result = kevent(kfd, NULL, 0, getEvent, 10, &pollingTime);
		if (result == -1) {
			std::cout << "kevent() error" << std::strerror(errno) << std::endl;
			return (1);
		} else if (result == 0)
			std::cout << "waiting..." << std::endl;
		setEvent.clear();

		for (int i = 0; i < result; i++) {
			if (getEvent[i].ident == ser_socket) { // 연결 성공?
				cli_socket = accept(ser_socket, (struct sockaddr *)&cli_addr, &cli_addr_len);
				if (cli_socket == -1) {
					std::cout << "accept() error" << std::strerror(errno) << std::endl;
					return (1);
				}
				std::cout << "fd INSERT" << std::endl;
				change_event(setEvent, cli_socket, EVFILT_READ, EV_ADD | EV_ENABLE);
				change_event(setEvent, cli_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE);
			} else if (getEvent[i].ident == EVFILT_READ) { // read
				read_len = read(getEvent[i].ident, buf, 1024);
				std::cout << "data : " << std::string(buf, read_len) <<  std::endl;
				if (read_len == 0) {
					std::cout << "client" << cli_socket << "disconnected" << std::endl;
					close(cli_socket);
				} else if (read_len == -1) {
					std::cout << "read error : " << std::strerror(errno) << std::endl;
					close(cli_socket);
					close(ser_socket);
					return (1);
				}
				change_event(setEvent, getEvent[i].ident, EVFILT_READ, EV_ADD | EV_DISABLE);
				change_event(setEvent, getEvent[i].ident, EVFILT_WRITE, EV_ADD | EV_ENABLE);
			} else if (getEvent[i].ident == EVFILT_WRITE) { // write
				if (write(getEvent[i].ident, httpTestHeaderString.data(), httpTestHeaderString.length()) == -1) {
					std::cout << "write error : " << std::strerror(errno) << std::endl;
					return (1);
				}
				change_event(setEvent, getEvent[i].ident, EVFILT_WRITE, EV_ADD | EV_DISABLE);
				change_event(setEvent, getEvent[i].ident, EVFILT_READ, EV_ADD | EV_ENABLE);
			}
		}
	}
	close(ser_socket);
	return (0);
}
