#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	int s_server, s_client;
	struct sockaddr_in addr_server, addr_client;
	char buffer[1024];
	socklen_t address_size;

	s_server = socket(PF_INET, SOCK_STREAM, 0);
	if (s_server == -1)
	{
		std::cout << "SOCKET ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(8080);

	if (bind(s_server, (struct sockaddr*)&addr_server, sizeof(addr_server)) == -1)
	{
		std::cout << "BIND ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	if (listen(s_server, 4) == -1)
	{
		std::cout << "LISTEN ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	address_size = sizeof(addr_client);
	s_client = accept(s_server, (struct sockaddr*)&addr_client, &address_size);
	if (s_client == -1)
	{
		std::cout << "ACCEPT ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	write(s_client, "YEAH", 4);
	close(s_client);
	close(s_server);

	return (0);
}
