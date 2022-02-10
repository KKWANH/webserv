#ifndef CONNECTIONSOCKET_HPP
# define CONNECTIONSOCKET_HPP

#include <iostream>

#include "Socket.hpp"
#include "./httpHandler/HTTPRequestHandler.hpp"
#include "./httpHandler/HTTPResponseHandler.hpp"
#include "./httpHandler/HTTPData.hpp"

class ConnectionSocket : puclie Socket {
	private:
		HTTPRequestHandler			*_req;
		HTTPResponseHandler			*_res;
		HTTPData					*_data;
		NginxConfig::GlobalConfig	_nginxConf;
		long						_dynamicBufSize;
		bool						_connectionCloseByServer;
		struct sockaddr_in			_serverAddr;
		struct sockaddr_in			_clientAddr;

	public:
		ConnectionSocket(int listeningSocket, const NginxConfig::ServerBlock &conf, const NginxConfig::GlobalConfig &nginxConf) : Socket(-1, serverConf), _nginxConf(nginxConf)
		{
			socklen_t	socketLen;
			this->_socket = accept(listeningSocket, (struct sockaddr *)&this->_clientAddr, &socketLen);
			if (this->_socket == -1)
				throw std::string("Accept error");
			if (getsockname(_socket, (struct sockaddr *)&this->_serverAddr, &socketLen) == -1)
				throw std::string("Getsockname error");
			_req = new HTTPRequestHandler(_socket, _serverConf, _nginxConf);
			_res = NULL;
			_dynamicBufSize = 0;
			_connectionCloseByServer = false;
			_data = new HTTPData();
			setConnectionData(_clienAddr, _serverAddr);
		}

		HTTPRequestHandler::Phase	HTTPRequestProcess(void)
		{

		}

		HTTPResponseHandler::Phase	HTTPResponseProcess(void)
		{

		}

		static void					connectionSocketKiller(void *connectionSocket)
		{

		}

		int							runSocket(void)
		{

		}

		int							getCGIfd(void)
		{

		}

		int							getFiledfd(void)
		{

		}

		long						getDynamicBufSize(void)
		{

		}

		void						setConnectionData(struck sockaddr_in _serverSocketAddr, struct sockaddr_in _clientSocketAddr)
		{

		}

		void						setDynamicBufSize(long dynamicBufSize)
		{

		}
};

#endif
