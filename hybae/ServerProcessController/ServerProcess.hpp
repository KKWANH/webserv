#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "./../HTTPMessageController/HTTPMessageController.hpp"
#include "./../SocketController/SocketController.hpp"
#include "./../KernelQueueController/KernelQueueController.hpp"
#include "./../ParsingController/ConfigController.hpp"
#include <dirent.h>
#include <sys/stat.h>

# define TEMP_BUFSIZ 1024

extern ConfigController config;

class ServerProcess {
	public:
		static int				serverProcess(SocketController* Socket, KernelQueueController* Kqueue) {
			while (true) {
				// queue에 event 적용
				Kqueue->setPollingCount(
					kevent(Kqueue->getKq(),
					Kqueue->getChangeList(),
					Kqueue->getChangeCount(),
					Kqueue->getEventList(),
					BUFSIZ,
					NULL)
				);

				// 적용된 event 초기화
				Kqueue->clearChangeList();
				Kqueue->resetChangeCount();

				// queue내 남아있는 이벤트만큼 반복
				for (int i = 0; i < Kqueue->getPollingCount(); i++) {
					if (Kqueue->getEventList(i)->filter == EVFILT_READ) {
						std::cout << "[READ]\t";
						// server read
						if ((int)Kqueue->getEventList(i)->ident == Socket->getSocketServer()) {
							Socket->setSocketClient(accept(Socket->getSocketServer(), Socket->getConvertedAddressClient(), Socket->getSocketLength()));
							fcntl(Socket->getSocketClient(), F_SETFL, O_NONBLOCK);
							Kqueue->setReadKqueue(Socket->getSocketClient());
							std::cout << "Server connect : [" << Socket->getSocketClient() << "]" << std::endl;
						}
						// TODO: file 크기가 큰 경우 나눠서 통신하는 기능 구현
						// client read
						else {
							
							int fd = Kqueue->getEventList(i)->ident;
							char buf[TEMP_BUFSIZ];
							int n;
							n = read(fd, buf, TEMP_BUFSIZ - 1);
							std::cout << "N : " << n << std::endl;
							if (n == -1) {
								std::cout << "RECV ERROR" << std::endl;
								return (-1);
							}
							else if (n == TEMP_BUFSIZ - 1) {
								buf[n] = '\0';
								Kqueue->sumMessage(fd, buf);
							}
							else {
								buf[n] = '\0';
								Kqueue->sumMessage(fd, buf);
								std::cout << "GET ALL" << std::endl;
								if (Kqueue->addRequestMessage(fd) == -1)
								return (-1);
								Kqueue->setWriteKqueue(fd);
								std::string temp = ResponseMessage::setResponseMessage(Kqueue->getRequestMessage(fd));
								Kqueue->saveResponseMessage(fd, temp);
							}
						}
					}
					// write
					else if (Kqueue->getEventList(i)->filter == EVFILT_WRITE) {
						int fd = Kqueue->getEventList(i)->ident;
						std::cout << "[WRITE]\tmessage : [" << fd << "]" << std::endl;
						if (Kqueue->writeResponseMessage(fd, TEMP_BUFSIZ) != TEMP_BUFSIZ) {
							std::cout << "SUCCESS" << std::endl;
							Kqueue->removeRequestMessage(fd);
							close(fd);
						}
					}
				}
			}
		};
};

#endif