#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "HTTPConnection.hpp"
#include "KernelQueueController.hpp"
#include "SocketController.hpp"
#include "ConfigBlocks.hpp"
#include "TimeController.hpp"
#include <cstring>
#include <iostream>

extern NginxConfig::GlobalConfig _config;

class ServerProcess {
	public:
		static void		serverProcess() {
			//int clientSocket;
			//char buffer[1024];
			std::string httpTestHeaderString;
			//int readLength;

			KernelQueueController kq(100);

			SocketController *socketController = new SocketController[_config._http._server.size()];
			for (int i = 0; i < (int)_config._http._server.size(); i++) {
				std::cout << "BIND PORT : " << atoi(_config._http._server[i]._dir_map["listen"].c_str()) << std::endl;
				socketController[i].generator(atoi(_config._http._server[i]._dir_map["listen"].c_str()));
				kq.addEvent(socketController[i].binding(), EVFILT_READ, &socketController[i]);
			}
			TimeController timer;

			while (true) {
				int events = kq.accessEvents();
				if (events) {
					for (int i = 0; i < events; i++) {
						try {
							ClassController* udata = reinterpret_cast<ClassController*>(kq.getInstanceByEventIndex(i));
							// 바인딩 처리
							if (dynamic_cast<SocketController*>(udata) != NULL) {
								SocketController* socketController = reinterpret_cast<SocketController*>(udata);
								int conn_socket = socketController->run();
								std::cout << "conn_socket :" << conn_socket << std::endl;
								int server_block = socketController->getServerBlockNum();
								if (server_block < 0)
								{
									close(conn_socket);
									throw ErrorHandler(__FILE__, __func__, __LINE__, "We can't find that block", ErrorHandler::NON_CRIT);
								}
								HTTPConnection* httpConnection = new HTTPConnection(conn_socket, server_block, socketController);
								kq.addEvent(conn_socket, EVFILT_READ, httpConnection);
								kq.addEvent(conn_socket, EVFILT_WRITE, httpConnection);
								kq.disableEvent(conn_socket, EVFILT_WRITE, httpConnection);
								timer.init_time(conn_socket);
								throw ErrorHandler(__FILE__, __func__, __LINE__, "test", ErrorHandler::NON_CRIT);
							}
							// HTTPConnection 처리
							else if (dynamic_cast<HTTPConnection*>(udata) != NULL) {
								HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);

								int fd = kq.getFdByEventIndex(i);
								if (kq.isCloseByEventIndex(i)) {
									std::cout << "Client closed socket" << std::endl;
									timer.del_time(kq.getFdByEventIndex(i));
									delete hc;
								}
								else {
									//std::cout << "---------[fd : " << fd << "]";
									int result = hc->run();
									if (result == HTTPConnection::REQUEST_TO_RESPONSE) {
										// READ -> WRITE
										std::cout << "kq(r) : " << fd << std::endl;
										kq.disableEvent(fd, EVFILT_READ, udata);
										kq.enableEvent(fd, EVFILT_WRITE, udata);
										timer.clean_time(fd);
									} else if (result == HTTPConnection::READY_TO_FILE) {
										kq.addEvent(hc->getFileFd(), EVFILT_READ, udata);
										kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
									} else if (result == HTTPConnection::FILE_READ) {
										/*
										std::cout << " WRITE----------" << std::endl;
										std::cout << "[FILE FD] : " << hc->getFileFd() << std::endl;
										std::cout << "[socket FD] : " << hc->getSocketFd() << std::endl;
										std::cout << "[DATA SIZE] : " << kq.getDataSize(i) << std::endl;
										std::cout << "---------------------------" << std::endl;
										*/
										kq.enableEvent(hc->getFileFd(), EVFILT_READ, udata);
										kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
									} else if (result == HTTPConnection::FILE_WRITE) {
										/*
										std::cout << " READ----------" << std::endl;
										std::cout << "[FILE FD] : " << hc->getFileFd() << std::endl;
										std::cout << "[socket FD] : " << hc->getSocketFd() << std::endl;
										std::cout << "[DATA SIZE] : " << kq.getDataSize(i) << std::endl;
										std::cout << "---------------------------" << std::endl;
										*/
										kq.disableEvent(hc->getFileFd(), EVFILT_READ, udata);
										kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
									} else if (result == HTTPConnection::CLOSE) {
										// 이벤트 제거
										std::cout << "kq(w) : " << fd << std::endl;
										delete hc;
										timer.del_time(fd);
									}
								}
							}
						}
						//timer.check_time();
						catch (const ErrorHandler& err) {
							int fd = kq.getFdByEventIndex(i);
							timer.del_time(fd);
							if (fd > 5)
								close(fd);
							//근데 소켓 연결시에는 알 도리가 없으니..
							//거기서 직접 해제를 해야 할듯
							//delete hc는 어디에
							//근데 없어도 누수가 안남...
							std::cerr << err.what() << std::endl;
							if (err.getLevel() == ErrorHandler::CRIT)
								exit(1);
						}
					}
				} else {
					std::cout << "waiting..." << std::endl;
				}
			}
			return;
		}
};

#endif
