#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include <fcntl.h>

#include "Socket.hpp"
// #include "Utils.hpp"

class ListeningSocket : public Socket {
	private:
		int					_backlog;
		int					_port;
		const char			*_ip;
		struct sockaddr_in	_sockAddr;

	public:
		ListeningSocket(const NginxConfig::ServerBlock &serverConfig, int backlog) : Socket(-1, serverConfig)
		{
			_port = std::atoi(_serverConf.dirMap.find("listen").c_str());
			_backlog = backlog;
		}

		void	setSocket(void)
		{
			char	optval[1024];

			this->_socket = socket(PF_INET, SOCK_STREAM, 0);
			setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, optval, sizeof(optval)); // 소켓의 송수진 동작 옵션을 제어할 수 있다.
			// SOL_SOCKET 레벨에서 SO_REUSEADDR를 사용한다는 뜻 = 이미 사용된 주소를 재사용 하도록 해줌

			if (this->_socket == -1)
				throw std::string("Socket error"); // ErrorHander로 바꾸기
		}

		void	setSocketAddr(void)
		{
			std::memset(&_sockAddr, 0, sizeof(_sockAddr));
			_sockAddr.sin_family = AF_INET;
			_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			_sockAddr.sin_port = htons(_port);
		}

		void	bindSocket(void)
		{
			if ((bind(_socket, (struct sockaddr*)&_sockAddr, sizeof(_sockAddr))) == -1)
				throw std::string("Bind error");
		}

		void	listenSocket(void)
		{
			if (listen(_socket, _backlog) == -1)
				throw std::string("Listen error");
		}

		int		runSocket(void)
		{
			this->setSocket();
			this->setSocketAddr();
			this->bindSocket();
			this->listenSocket();
			return (0);
		}
};

#endif
