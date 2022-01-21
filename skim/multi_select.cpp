#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

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

	std::vector<int>	fdList;
	struct timeval		pollingTime;
	fd_set				readFdSet;
	int					result;

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

	// poll 시간 설정
	pollingTime.tv_sec = 1;
	pollingTime.tv_usec = 0;

	// read fd set 초기화
	FD_ZERO(&readFdSet);

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

	// accept를 하기위한 ser_socket도 select로 감시한다.
	fdList.push_back(ser_socket);
	std::sort(fdList.begin(), fdList.end());

	while (1)
	{
		for (std::vector<int>::iterator it = fdList.begin(); it != fdList.end(); it++)
			FD_SET(*it, &readFdSet);

		// 1) 검사할 파일 디스트립터 번호 중 가장 큰 값 + 1
		// 2) 일기를 검사할 fd_set
		// 3) 쓰기를 검사한 fd_set
		// 4) 예외를 검사할 fd_set
		// 5) 검사하는 시간
		// return : 성공시 성공한 파일 디스트립터 번호의 수, 실패시 -1
		result = select(fdList.back() + 1, &readFdSet, NULL, NULL, &pollingTime);
		if (result == -1) {
			std::cout << "select()" << std::strerror(errno) << std::endl;
			return (1);
		} else if (result == 0) {
			std::cout << "waiting..." << std::endl;
		} else { // select 된 socket fd를 찾아서 처리한다.
			for (std::vector<int>::iterator it = fdList.begin(); it != fdList.end(); it++) {
				int testfd = *it;
				if (FD_ISSET(testfd, &readFdSet)) {
					if (testfd == ser_socket) {
						cli_socket = accept(ser_socket, (struct sockaddr *)&cli_addr, &cli_addr_len);
						if (cli_socket == -1) {
							std::cout << "accept() error" << std::strerror(errno) << std::endl;
							return (1);
						}
						std::cout << "fd INSERT" << std::endl;
						fdList.push_back(cli_socket);
						std::sort(fdList.begin(), fdList.end());
					} else {
						read_len = read(testfd, buf, 1024);
						std::cout << "data : " << std::string(buf, read_len) << std::endl;
						write(testfd, httpTestHeaderString.data(), httpTestHeaderString.length());
						fdList.erase(it);
						FD_CLR(testfd, &readFdSet);
						close(testfd);
					}
					break;
				}
			}
		}
	}
	close(ser_socket);
	return (0);
}
