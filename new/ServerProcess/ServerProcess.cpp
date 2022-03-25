#include "ServerProcess.hpp"
#include <iostream>

extern NginxConfig::GlobalConfig _config;

void	ServerProcess::serverProcess() {
	KernelQueueController kq(100);
	int	serverFd;
	SocketController *socketController = new SocketController[_config._http._server.size()];

	for (int i = 0; i < (int)_config._http._server.size(); i++) {
		std::cout << "BIND PORT : " << atoi(_config._http._server[i]._dir_map["listen"].c_str()) << std::endl;
		socketController[i].generator(atoi(_config._http._server[i]._dir_map["listen"].c_str()));
		serverFd = socketController[i].binding();
		if (serverFd != 0)
			kq.addEvent(serverFd, EVFILT_READ, &socketController[i]);
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
						std::stringstream	clientPortString;
						std::stringstream	hostPortString;

						std::string	client_ip = std::string(inet_ntoa((socketController->getServerAddr()).sin_addr));
						clientPortString << ntohs(socketController->getServerAddr().sin_port);

						std::string host_ip = std::string(inet_ntoa((socketController->getClientAddr()).sin_addr));
						hostPortString << ntohs(socketController->getClientAddr().sin_port);

						int server_port = (int)ntohs((socketController->getServerAddr()).sin_port);
						HTTPConnection* httpConnection = new HTTPConnection(conn_socket, server_block, server_port, client_ip, std::string(clientPortString.str()), host_ip, std::string(hostPortString.str()));
						if (fcntl(conn_socket, F_SETFL, O_NONBLOCK) == -1)
							exit(-1);
						kq.addEvent(conn_socket, EVFILT_READ, httpConnection);
						kq.addEvent(conn_socket, EVFILT_WRITE, httpConnection);
						kq.disableEvent(conn_socket, EVFILT_WRITE, httpConnection);
						int keepalive_timeout = -1;
						if (_config._http._dir_map["keepalive_timeout"].size() > 0)
							keepalive_timeout = atoi(_config._http._dir_map["keepalive_timeout"].c_str());
						if (_config._http._server[server_block]._dir_map["keepalive_timeout"].size() > 0)
							keepalive_timeout = atoi(_config._http._server[server_block]._dir_map["keepalive_timeout"].c_str());
						else if (keepalive_timeout < 0)
							keepalive_timeout = 60;
						timer.init_time(conn_socket, httpConnection, keepalive_timeout);
						std::cout << "conn :" << conn_socket << std::endl;
					}
					// HTTPConnection 처리
					else if (dynamic_cast<HTTPConnection*>(udata) != NULL) {
						HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);

						int fd = kq.getFdByEventIndex(i);
						if (kq.isCloseByEventIndex(i) && fd == hc->getSocketFd()) {
							std::cout << "Client closed socket : " << fd << std::endl;
							timer.del_time(hc->getSocketFd());
							delete hc;
							//continue ;
						}
						else {
							int result = hc->run();
							if (result == HTTPConnection::REQUEST_TO_RESPONSE) {
								kq.disableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::BODY_TO_RESPONSE) {
								kq.disableEvent(hc->getCgiInputFd(), EVFILT_WRITE, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::READY_TO_MESSAGE_BODY) {
								kq.disableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								kq.addEvent(hc->getCgiInputFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::MESSAGE_BODY_READ) {
								kq.enableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getCgiInputFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::MESSAGE_BODY_WRITE) {
								kq.disableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getCgiInputFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::READY_TO_CGI) {
								kq.addEvent(hc->getCgiOutputFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CGI_READ) {
								kq.enableEvent(hc->getCgiOutputFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CGI_WRITE) {
								kq.disableEvent(hc->getCgiOutputFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::READY_TO_FILE) {
								kq.addEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::FILE_READ) {
								kq.enableEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::FILE_WRITE) {
								kq.disableEvent(hc->getFileFd(), EVFILT_READ, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
							} else if (result == HTTPConnection::CLOSE) {
								// 이벤트 제거
								// std::cout << "kq(w) : " << fd << std::endl;
									timer.del_time(hc->getSocketFd());
									delete hc;
									//continue ;
							} else if (result == HTTPConnection::RE_KEEPALIVE) {
								// std::cout << "re" << std::endl;
								kq.disableEvent(hc->getSocketFd(), EVFILT_WRITE, udata);
								kq.enableEvent(hc->getSocketFd(), EVFILT_READ, udata);
								timer.clean_time(hc->getSocketFd());
							}
						}
					}
				}
				catch (const ErrorHandler& err) {
					ClassController* udata = reinterpret_cast<ClassController*>(kq.getInstanceByEventIndex(i));
					HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);
					int fd = hc->getSocketFd();
					timer.del_time(fd);
					// std::cout << " fd : " << fd << std::endl;
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
			// std::cout << "waiting..." << std::endl;
		}
		timer.check_time(HTTPConnection::killConnection);
	}
	return;
};
