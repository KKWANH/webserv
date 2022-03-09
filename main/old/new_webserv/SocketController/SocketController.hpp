#ifndef SOCKETCONTROLLER_HPP
# define SOCKETCONTROLLER_HPP
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <ctime>
#include <cstdlib>
#include <random>
#include <fcntl.h>
#include "ErrorHandler.hpp"
#include "../Process.hpp"

#define PORT 9090

class SocketController : public Process {
	private:
		int								socket_server, socket_client;
		struct sockaddr_in				address_server, address_client;
		socklen_t						c_len;
		int								bind_check, listen_check, blocking_check;

	public:
		int								getSocketServer()					{ return (this->socket_server); }
		int								getSocketClient()					{ return (this->socket_client); }
		struct sockaddr_in				getAddressServer()					{ return (this->address_server); }
		struct sockaddr_in				getAddressClient()					{ return (this->address_client); }
		socklen_t*						getSocketLength()	{
			c_len = (sizeof(address_client));
			return (&c_len);
		}

		SocketController(int port) {
			// Create Socket on Server
			socket_server = socket(PF_INET, SOCK_STREAM, 0);

			// Init Address Informaion
			memset(&address_server, 0, sizeof(address_server));
			address_server.sin_addr.s_addr = htonl(INADDR_ANY);
			address_server.sin_family = AF_INET;
			address_server.sin_port = htons(port);
		//	address_server.sin_port = htons(getNanSu());

			// Bind Server Socket to Server Address
			bind_check = bind(socket_server, (struct sockaddr*)&address_server, sizeof(address_server));
			if (bind_check == -1) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Fail Binding");
			}

			// Start to listen
			listen_check = listen(socket_server, 10);
			if (listen_check == -1) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Fail Listen");
			}

			blocking_check = fcntl(this->getSocketClient(), F_SETFL, O_NONBLOCK);
			if (blocking_check == -1) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Fail Non-Block");
			}
			return ;
		}

		int								run(void) {
			return (accept(socket_server, (struct sockaddr*)&this->address_client, this->getSocketLength()));
		}

		/*
		int								getNanSu() {
			srand((unsigned int)time(NULL));
			int res = rand();
			res = res % 100;
			std::cout << "your port is : " << 9000 + res << std::endl;
			return (9000 + res);
		}
		*/
};

#endif
