#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include "ErrorHandler.hpp"
#include "NginxConfig.hpp"
#include "Utils.hpp"

class Socket {
    protected:
        int _socket;
        NginxConfig::ServerBlock _serverConf;

    Public:
        Socket(int socket_);
        Socket(int socket_, const Nginxconfig::ServerBlock& serverConf);
        virtual ~Socket();
        int getSocket() const;
        NginxConfig::serverBlock getConfig() const;
        virtual int runSocket() = 0;
};


Socket::Socket(int socket_) : _socket(socket_)
{
}

Socket::Socket(int socket_, const NginxConfig::ServerBlock& serverConf) : _socket(socket_), _serverConf(serverConf)
{
}

Socket::~Socket() {
    if (_socket > 0) {
        if (close(_socket) == -1) {
            throw ErrorHandler("Error: Can't close socket", ErrorHandler::ALERT, "Socket::~Socket");
        }
    }
}

int Socket::getSocket() const {
    return _socket;
}

NginxConfig::ServerBlock Socket::getConfig() const {
    return _serverConf;
}



#endif
