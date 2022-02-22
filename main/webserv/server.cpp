#include "./ParsingController/ConfigController.hpp"
#include "./SocketController/SocketController.hpp"
#include "./ServerProcessController/ServerProcess.hpp"
#include "./KernelQueueController/KernelQueueController.hpp"
#include "./ParsingController/MIMEController.hpp"
#include "./ErrorHandler/ErrorHandler.hpp"

ConfigController config;
extern ConfigController config;
MIMEController mime;
extern MIMEController mime;

int main(int argc, char **argv) {
	SocketController Socket;
	try{
	// config file setting
	if (config.setConfig(argc, argv) == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "config file setting error");

	// mime types setting
	if (mime.setMIME() == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "응 에러 나봐~~~ 리팩토링하면돼~~~");

	// socket communication init
	if (Socket.init() == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "응 에러 나봐~~~ 리팩토링하면돼~~~");

	// polling init
	KernelQueueController Kqueue;
	if (Kqueue.init(Socket.getSocketServer()) == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "응 에러 나봐~~~ 리팩토링하면돼~~~");

	// non-blocking socket communication 
	if (ServerProcess::serverProcess(&Socket, &Kqueue) == -1)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "응 에러 나봐~~~ 리팩토링하면돼~~~");
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		close(Socket.getSocketServer());
		return (-1);
	}

	close(Socket.getSocketServer());
	return (0);
}
