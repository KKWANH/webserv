#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#include "./ConfigController.hpp"
#include "./HTTPMessageController.hpp"
#include "./SocketController.hpp"
#include "./ServerProcess.hpp"

ConfigController config;
extern ConfigController config;

int 			main(void) {
	// 설정 파일 읽어 클래스 내 정보 저장
	if (config.setConfig() == -1)
		return (-1);
	//config.printConfig();

	SocketController	socketController;
	socklen_t					c_len;

	if (socketController.init() == -1)
		return (-1);
	
	while(true) {
		c_len = sizeof(socketController.getAddressClient());
		socketController.setSocketClient(accept(socketController.getSocketServer(), socketController.getConvertedAddressClient(), &c_len));
		RequestMessage requestMessage;
		if (RequestMessage::parsingRequestMessage(socketController.getSocketClient(), &requestMessage) == -1)
			continue;
		std::string msg = ServerProcess::serverProcess(socketController.getSocketClient(), &requestMessage);
		write(socketController.getSocketClient(), msg.c_str(), msg.length());
		close(socketController.getSocketClient());
	}
	close(socketController.getSocketServer());
	return (0);
}