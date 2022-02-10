#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 9090
#define IPADDR "127.0.0.1"
#define BUFFER_SIZE 1024

int main(void) {
	int c_socket;
	struct sockaddr_in c_addr;
	int len, n;

	char rcvBuffer[BUFFER_SIZE];
	const char *buffer;

	c_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(IPADDR);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(PORT);

	if(connect(c_socket, (struct sockaddr*)&c_addr, sizeof(c_addr)) == -1) {
		std::cout << "Could not Connect" << std::endl;
		close(c_socket);
		return (-1);
	}

	//scanf("%s", buffer);
	//buffer[strlen(buffer)] = '\0';
	std::string temp = "GET /temp HTTP/1.1\r\n";
	write(c_socket, temp.c_str(), temp.size());

	n = recv(c_socket, rcvBuffer, BUFSIZ, 0);
	if (n < 0) {
		return (-1);
	}

	rcvBuffer[n] = '\0';
	std::cout << "Received Data : " << rcvBuffer << std::endl;
	
	close(c_socket);
	return (1);
}