#include "ConfigController.hpp"
#include "SocketController.hpp"
#include "ServerProcess.hpp"
#include "KernelQueueController.hpp"
#include "MIMEController.hpp"
#include "ErrorHandler.hpp"

ConfigController config;
extern ConfigController config;
MIMEController mime;
extern MIMEController mime;

int main(int argc, char **argv) {
	SocketController Socket;

	try {
		// config file setting
		config.setConfig(argc, argv);

		// mime types setting
		mime.setMIME();

		// socket communication init
		Socket.init();

		// polling init
		KernelQueueController Kqueue;
		Kqueue.init(Socket.getSocketServer());

		// non-blocking socket communication 
		ServerProcess::serverProcess(&Socket, &Kqueue);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		close(Socket.getSocketServer());
		return (-1);
	}
	close(Socket.getSocketServer());
	return (0);
}
