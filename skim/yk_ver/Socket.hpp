#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

// #include "ErrorHandler.hpp"
#include "./configHandler/NginxConfig.hpp"

class Socket {
	protected:
		int							_socket;
		NginxConfig::ServerBlock	_serverConf; // nginx config 파일
	public:
		Socket(int socket) : _socket(socket) {}

		Socket(int socket, const NginxConfig::ServerBlock &serverConf) : _socket(socket), _serverConf(serverConf) {}

		virtual						~Socket()
		{
			if (_socket > 0) {
				if (close(_socket) == -1)
					throw std::string("Close error"); // ErrorHander로 바꾸기
			}
		}

		int							getSocket(void) const
		{
			return _socket;
		}

		NginxConfig::ServerBlock	getConfig(void) const
		{
			return _serverConf;
		}

		virtual int					runSocket(void) = 0;
};

#endif
