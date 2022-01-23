#ifndef SOCKETCONTROLLER_HPP
# define SOCKETCONTROLLER_HPP
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#define PORT 9090

class SocketController {
	private:
		int									socket_server, socket_client;
		struct sockaddr_in	address_server, address_client;
		socklen_t						c_len;
		int									bind_check, listen_check;
	
	public:

		int									getSocketServer() {
			return (this->socket_server);
		}

		int									getSocketClient() {
			return (this->socket_client);
		}

		struct sockaddr_in	getAddressServer() {
			return (this->address_server);
		}

		struct sockaddr_in	getAddressClient() {
			return (this->address_client);
		}

		struct sockaddr*		getConvertedAddressClient() {
			return ((struct sockaddr*)&this->address_client);
		}

		void								setSocketClient(int accept) {
			this->socket_client = accept;
			return ;
		}

		int									init() {
			// Create Socket on Server
			socket_server = socket(PF_INET, SOCK_STREAM, 0);

			// Init Address Informaion
			memset(&address_server, 0, sizeof(address_server));
			address_server.sin_addr.s_addr = htonl(INADDR_ANY);
			address_server.sin_family = AF_INET;
			address_server.sin_port = htons(PORT);
			
			// Bind Server Socket to Server Address
			bind_check = bind(socket_server, (struct sockaddr*)&address_server, sizeof(address_server));
			if (bind_check == -1) {
				std::cout << "Fail Binding\n" << std::endl;
				return (-1);
			}

			// Start to listen
			listen_check = listen(socket_server, 10);
			if (listen_check == -1) {
				std::cout << "Fail Listen\n" << std::endl;
				return (-1);
			}
			return (0);
		}
};

#endif