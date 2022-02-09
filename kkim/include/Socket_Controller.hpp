#ifndef					SOCKET_CONTROLLER_HPP
# define				SOCKET_CONTROLLER_HPP

# include				<string>
# include				<iostream>
# include				<sys/socket.h>
# include				<netinet/in.h>
# include				<arpa/inet.h>

# include				"Utils.hpp"

# define				DEFAULT_PORT	9090

/**
* -------------------------------------------------------------
* > SocketController
*
* (constructor) (bool):	Set private resources, and discriminate either the mime or the config
*
* - Member functions:
* Setter:				SocketClient
* Getter:				SocketClient, SocketServer, AddressClient, AddressServer, Cnv_AddressClient, SocketLength
* init:					initilize socket communication
* ------------------------------------------------------------- *
*/

class					SocketController
{
	private:
		int
			_soc_s, _soc_c;
		struct sockaddr_in
			_add_s, _add_c;
		socklen_t
			_len_c;
		int
			_chk_bind, _chk_listen;
		
	public:
		void			setSocketClient(int _acc)		{ this->_soc_c = _acc; };
		int				getSocketServer(void)			{ return (_soc_s); };
		int				getSocketClient(void)			{ return (_soc_c); };
		struct sockaddr_in
						getAddrServer(void)				{ return (_add_s); };
		struct sockaddr_in
						getAddrClient(void)				{ return (_add_c); };
		struct sockaddr*
						getConvertedAddressClient(void)	{ return ((struct sockaddr*)&_add_c); };
		socklen_t*		getSocketLength(void)			{ _len_c = (sizeof(_add_c)); return (&_len_c); };


		int
			init(void)
		{
			// Create Socket on Server
			// - PF_INET is for Protocol, AF_INET is for Address.
			// - Actually swapping PF_INET and AF_INET makes no difference..
			// - SOCK_STREAM is for TCP, SOCK_DGRAM is for UDP.
			_soc_s = socket(PF_INET, SOCK_STREAM, 0);

			// Init Address Informaion
			// - htonl : Host to Network Long
			// - INADDR_ANY : Automatically use the IP of the available LAN card in this computer.
			// - htons : Host to Network Short
			memset(&_add_s, 0, sizeof(_add_s));
			_add_s.sin_addr.s_addr = htonl(INADDR_ANY);
			_add_s.sin_family = AF_INET;
			_add_s.sin_port = htons(DEFAULT_PORT);

			// Bind Server Socket to Server Address
			_chk_bind = bind(_soc_s, (struct sockaddr*)&_add_s, sizeof(_add_s));
			if (_chk_bind == ERROR)
			{
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Bind error" << std::endl;
				return (ERROR);
			}

			// Listen
			_chk_listen = listen(_soc_s, 10);
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