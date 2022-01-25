#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
	int sock, str_len;

	struct sockaddr_in addr;
	char buf[1024];

	//소켓을 만듦
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cout << "LISTEN ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	//클라의 주소를 초기화 및 설정
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);

	//서버에게 연결요청
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cout << "CONNECT ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	//서버로부터 뭔갈 받음
//	read(sock, buf, sizeof(buf));
//	std::cout << "WELCOME : " << buf << std::endl;

	char qurey[1024];
	int len = 0;
	while (1)
	{
		std::cout << "용건이 뭔데? 끝꺼면 q만 치십쇼 : " ;
		std::cin >> qurey;
		if (strcmp(qurey, "q") == 0)
		{
			std::cout << "끝" << std::endl;
			break ;
		}
		send(sock, qurey, strlen(qurey), 0);
		std::cout << " 난 이리 보냄 " << qurey << std::endl;
		len = recv(sock, qurey, 1024 -1, 0);
		qurey[len] = 0;
		std::cout << "서버가 말하길 : " << qurey << std::endl;
	}
	std::cout << "끝" << std::endl;
	close(sock);
	return (0);
}
