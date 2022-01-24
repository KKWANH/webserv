#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#include "./configController.hpp"
#include "./socketController.hpp"
#include "./requestMsgController.hpp"
#include "./serverProcess.hpp"

configController config;
extern configController		config;

int		main(void)
{
	// config 파일에 대한 설정을 해준다.
	if (config.setConfig() == -1)
		return (-1);
	// config.printConfig();

	socketController	socketController;
	if (socketController.init() == -1)
		return (-1);

	socklen_t	len;
	while (1) {
		len = sizeof(socketController.getClientAddress());
		socketController.setClientSock(accept(socketController.getServerSocket(), socketController.getConvertAddressClient(), &len));
		requestMsg	requestMsg;
		if (requestMsg::parsingRequestMsg(socketController.getClientSocket(), &requestMsg) == -1)
			return (-1);
		std::string msg = serverProcess::process(socketController.getClientSocket(), &requestMsg);
		write(socketController.getClientSocket(), msg.c_str(), msg.length());
		close(socketController.getClientSocket());
	}
	close(socketController.getServerSocket());
	return (0);
}
