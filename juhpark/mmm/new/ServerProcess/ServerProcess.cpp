#include "ServerProcess.hpp"
#include <iostream>

extern NginxConfig::GlobalConfig _config;

void	ServerProcess::serverProcess() {
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
						int server_block = socketController->getServerBlockNum();
						if (server_block < 0)
						{
							close(conn_socket);
							throw ErrorHandler(__FILE__, __func__, __LINE__, "We can't find that block", ErrorHandler::NON_CRIT);
						}
						int server_port = (int)ntohs((socketController->getServerAddr()).sin_port);
						std::string	client_ip = std::string(inet_ntoa((socketController->getClientAddr()).sin_addr));
						HTTPConnection* httpConnection = new HTTPConnection(conn_socket, server_block, server_port, client_ip);
						if (fcntl(conn_socket, F_SETFL, O_NONBLOCK) == -1)
							exit(-1);
						kq.addEvent(conn_socket, EVFILT_READ, httpConnection);
						kq.addEvent(conn_socket, EVFILT_WRITE, httpConnection);
						kq.disableEvent(conn_socket, EVFILT_WRITE, httpConnection);
						timer.init_time(conn_socket);
						std::cout << "conn :" << conn_socket << std::endl;
					}
					// HTTPConnection 처리
					else if (dynamic_cast<HTTPConnection*>(udata) != NULL) {
				//		std::cout << "re2" << std::endl;
						HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);

						int fd = kq.getFdByEventIndex(i);
						if (kq.isCloseByEventIndex(i)) {
							std::cout << "Client closed socket : " << fd << std::endl;
							timer.del_time(hc->getSocketFd());
							delete hc;
							continue ;
						}
						else {
							int result = hc->run();
							if (result == HTTPConnection::REQUEST_TO_RESPONSE) {
								// READ -> WRITE
								//std::cout << "kq(r) : " << fd << std::endl;
								std::cout << "a" << std::endl;
								kq.disableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::READY_TO_CGI) {
						//		std::cout << "b" << std::endl;
								kq.addEvent(hc->getCgiFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CGI_READ) {
						//		std::cout << "c" << std::endl;
								kq.enableEvent(hc->getCgiFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CGI_WRITE) {
						//		std::cout << "d" << std::endl;
								kq.disableEvent(hc->getCgiFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::READY_TO_FILE) {
						//		std::cout << "e" << std::endl;
								kq.addEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::FILE_READ) {
						//		std::cout << "f" << std::endl;
								kq.enableEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::FILE_WRITE) {
						//		std::cout << "g" << std::endl;
								kq.disableEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CLOSE) {
								// 이벤트 제거
								//td::cout << "kq(w) : " << fd << std::endl;
								std::cout << "bye" << std::endl;
								timer.del_time(hc->getSocketFd());
								delete hc;
								continue ;
							} else if (result == HTTPConnection::RE_KEEPALIVE) {
								// 이벤트 제거
								//td::cout << "kq(w) : " << fd << std::endl;
								std::cout << "re" << std::endl;
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
								kq.removeEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.removeEvent(hc->getCgiFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								timer.clean_time(hc->getSocketFd());
							}
						}
					if (timer.check_time(hc->getSocketFd(), hc->getServerBlock())) {
						std::cout << "bye" << std::endl;
						timer.del_time(hc->getSocketFd());
						delete hc;
						}
					}
				}
				/*
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
				*/
				catch (const ErrorHandler& err) {
					ClassController* udata = reinterpret_cast<ClassController*>(kq.getInstanceByEventIndex(i));
					HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);
					int fd = hc->getSocketFd();
					timer.del_time(fd);
					//아마 요 사이에 에러 페이지를 만들고 보내는 코드가 추가되어야 할듯함(였던거)
					if (fd > 5)
					{
					//	close(hc->getSocketFd());
					//	if (hc->getFileFd() > 0)
					//		close (hc->getFileFd());
						delete hc;
					}
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
};
