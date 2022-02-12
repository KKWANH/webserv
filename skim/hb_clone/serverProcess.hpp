#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include <dirent.h>
#include <sys/stat.h>

#include "requestMsgController.hpp"
#include "responseMsgController.hpp"
#include "socketController.hpp"
#include "KernelQueueController.hpp"
#include "configController.hpp"

extern configController		config;

class serverProcess {
	public:
		static int		ServerProcess(socketController *socket, KernelQueueController *kqueue)
			{
				while (1)
				{
					kqueue->setPollingCount(
						kevent(kqueue->getKq(),
						kqueue->getChangeList(),
						kqueue->getChangeCount(),
						kqueue->getEventList(),
						BUFSIZ,
						NULL)
					);

					kqueue->clearChangeList();
					kqueue->resetChangeCount();

					for (int i = 0; kqueue->getPollingCount(); i++) {
						if (kqueue->getEventList(i)->filter == EVFILT_READ) {
							std::cout << "[READ]\t";

							if ((int)kqueue->getEventList(i)->ident == socket->getServerSocket()) {
								socket->setClientSock(accept(socket->getServerSocket(), socket->getConvertAddressClient(), socket->getSocketLength()));
								fcntl(socket->getClientSocket(), F_SETFL, O_NONBLOCK);
								kqueue->setReadKqueue(socket->getClientSocket());
								std::cout << " Server Connect : [ " << socket->getClientSocket() << " ]" << std::endl;
							}

							else {
								int 	fd = kqueue->getEventList(i)->ident;
								char	buf[BUFSIZ];
								recv(fd, buf, BUFSIZ, 0);
								if (kqueue->addRequestMsg(fd, buf) == -1)
									return (-1);
								kqueue->setWriteKqueue(fd);
							}
						}
						else if (kqueue->getEventList(i)->filter == EVFILT_WRITE) {
							int fd = kqueue->getEventList(i)->ident;
							std::cout << "[WRITE]\t[" << fd << "]" << std::endl;
							// kqueue->getRequestMessage(fd)->printRequestMsg();
							std::string msg = responseMsg::setResponseMsg(kqueue->getRequestMessage(fd));
							write(fd, msg.c_str(), msg.size());
							kqueue->removeRequestMessage(fd);
							close(fd);
						}
					}
				}
			}
};

#endif
