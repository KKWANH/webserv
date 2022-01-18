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
	char buf[30];

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cout << "LISTEN ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);

	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1);
	{
		std::cout << "CONNECT ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}


	read(sock, buf, sizeof(buf));
	std::cout << "server : " << buf << std::endl;
	close(sock);
	return (0);
}
