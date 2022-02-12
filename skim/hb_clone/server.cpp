#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#include "./configController.hpp"
#include "./MIMEController.hpp"
#include "./socketController.hpp"
#include "./KernelQueueController.hpp"
#include "./requestMsgController.hpp"
#include "./serverProcess.hpp"

configController		config;
extern configController	config;

MIMEController			mime;
extern MIMEController	mime;

int		main(int ac, char *av[])
{
	// config 파일에 대한 설정을 해준다.
	if (config.setConfig() == -1)
		return (-1);
	// config.printConfig();

	if (mime.setMIME() == -1)
		return (-1);


	socketController	socket;
	if (ac >= 2)
		socket.setPort(atoi(av[1]));
	else
		socket.setPort(9090);
	if (socket.init() == -1)
		return (-1);

	KernelQueueController	kqueue;
	if (kqueue.init(socket.getServerSocket()) == -1)
		return (-1);

	if (serverProcess::ServerProcess(&socket, &kqueue) == -1)
		return (-1);

	close(socket.getServerSocket());
	return (0);
}
