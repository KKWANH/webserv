#include "./ParsingController/ConfigController.hpp"
#include "./SocketController/SocketController.hpp"
#include "./ServerProcessController/ServerProcess.hpp"
#include "./KernelQueueController/KernelQueueController.hpp"
#include "./ParsingController/MIMEController.hpp"

ConfigController config;
extern ConfigController config;
MIMEController mime;
extern MIMEController mime;

int main(int argc, char **argv) {

	// config file setting
	if (config.setConfig(argc, argv) == -1)
		return (-1);

	// mime types setting
	if (mime.setMIME() == -1)
		return (-1);

	// socket communication init	
	SocketController Socket;
	if (Socket.init() == -1)
		return (-1);

	// polling init
	KernelQueueController Kqueue;
	if (Kqueue.init(Socket.getSocketServer()) == -1)
		return (-1);

	// non-blocking socket communication 
	if (ServerProcess::serverProcess(&Socket, &Kqueue) == -1)
		return (-1);

	close(Socket.getSocketServer());
	return (0);
}