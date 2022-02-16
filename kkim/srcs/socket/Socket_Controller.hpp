#ifndef					SOCKET_CONTROLLER_HPP
# define				SOCKET_CONTROLLER_HPP

# include				<string>
# include				<iostream>
# include				<sys/socket.h>
# include				<netinet/in.h>
# include				<arpa/inet.h>

# include				"../utils/Utils.hpp"

# define				DEFAULT_PORT	9090

/**
-------------------------------------------------------------
> SocketController

(constructor) (bool):	Set private resources, and discriminate either the mime or the config

- Member functions:
Setter:					SocketClient
Getter:					SocketClient, SocketServer, AddressClient, AddressServer, Cnv_AddressClient, SocketLength
getRandomPort			return randomanized port number
init:					initilize socket communication
------------------------------------------------------------- *
*/

class					SocketController
{
	private:
		int
			_socket_server, _socket_client;
		struct sockaddr_in
			_address_server, _address_client;
		socklen_t
			_len_c;
		int
			_chk_bind, _chk_listen;
		
	public:
		void			setSocketClient(int _acc)		{ this->_socket_client = _acc; };
		int				getSocketServer(void)			{ return (_socket_server); };
		int				getSocketClient(void)			{ return (_socket_client); };
		struct sockaddr_in
						getAddrServer(void)				{ return (_address_server); };
		struct sockaddr_in
						getAddrClient(void)				{ return (_address_client); };
		struct sockaddr*
						getConvertedAddressClient(void)	{ return ((struct sockaddr*)&_address_client); };
		socklen_t*		getSocketLength(void)			{ _len_c = (sizeof(_address_client)); return (&_len_c); };

		int
			getRandomPort(void)
		{
			srand((unsigned int)time(NULL));
			int _rst = rand();
			_rst = _rst % 100;
			std::cout << "PORT : " << _rst << " + " << 9000 + _rst << std::endl;
			return (9000 + _rst);
		}

		int
			init(void)
		{
			// Create Socket on Server
			// - PF_INET is for Protocol, AF_INET is for Address.
			// - Actually swapping PF_INET and AF_INET makes no difference..
			// - SOCK_STREAM is for TCP, SOCK_DGRAM is for UDP.
			_socket_server = socket(PF_INET, SOCK_STREAM, 0);

			// Init Address Informaion
			// - htonl : Host to Network Long
			// - INADDR_ANY : Automatically use the IP of the available LAN card in this computer.
			// - htons : Host to Network Short
			memset(&_address_server, 0, sizeof(_address_server));
			_address_server.sin_addr.s_addr = htonl(INADDR_ANY);
			_address_server.sin_family = AF_INET;
			_address_server.sin_port = htons(getRandomPort());

			// Bind Server Socket to Server Address
			_chk_bind = bind(_socket_server, (struct sockaddr*)&_address_server, sizeof(_address_server));
			if (_chk_bind == ERROR)
			{
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Bind error" << std::endl;
				return (ERROR);
			}

			// Listen
			_chk_listen = listen(_socket_server, 10);
			if (_chk_listen == ERROR)
			{
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Listen error" << std::endl;
				return (ERROR);
			}

			return (0);
		}
};

#endif