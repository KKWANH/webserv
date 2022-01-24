#ifndef SOCKETCONTROLLER_HPP
# define SOCKETCONTROLLER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#define PORT 9090

class socketController {
	private:
		int					server_sock, client_sock;
		struct sockaddr_in	server_addr, client_addr;
		socklen_t			addr_size;
	public:
		int					getServerSocket(void) {
			return (this->server_sock);
		}

		int					getClientSocket(void) {
			return (this->client_sock);
		}

		struct sockaddr_in	getServerAddress(void) {
			return (this->server_addr);
		}

		struct sockaddr_in	getClientAddress(void) {
			return (this->client_addr);
		}

		struct sockaddr*	getConvertAddressClient(void) {
			return ((struct sockaddr*)&this->client_addr);
		}

		void				setClientSock(int accept) {
			this->client_sock = accept;
			return ;
		}

		int					init(void) {
			//ipv4, TCP 소켓 생성
			server_sock = socket(PF_INET, SOCK_STREAM, 0);

			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(PORT);

			// bint
			if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
			{
				std::cout << "Error: Fail binding" << std::endl;
				return (-1);
			}

			// listen
			if (listen(server_sock, 10) == -1)
			{
				std::cout << "Error: Fail listening" << std::endl;
				return (-1);
			}
			return (0);
		}
};

#endif
