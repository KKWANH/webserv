#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "HTTPConnection.hpp"
#include "KernelQueueController.hpp"
#include "SocketController.hpp"
#include "ConfigBlocks.hpp"
#include "ConfigController.hpp"
#include "TimeController.hpp"
#include <cstring>
#include <iostream>

extern ConfigController config;
extern NginxConfig::GlobalConfig _config;

class ServerProcess {
	public:
		static void		serverProcess() {
			//int clientSocket;
			//char buffer[1024];
			std::string httpTestHeaderString;
			//int readLength;

			KernelQueueController kq(100);

			std::cout << "SERVER BLOCK COUNT : " << _config._http._server.size() << std::endl;
			
			SocketController socketController[_config._http._server.size()];
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
						ClassController* udata = reinterpret_cast<ClassController*>(kq.getInstanceByEventIndex(i));

						// 바인딩 처리
						if (dynamic_cast<SocketController*>(udata) != NULL) {
							SocketController* socketController = reinterpret_cast<SocketController*>(udata);
							int conn_socket = socketController->run();
							std::cout << "conn_socket : " << conn_socket << std::endl;
							int server_block = socketController->getServerBlockNum();
							if (server_block < 0)
								throw ErrorHandler(__FILE__, __func__, __LINE__, "We can't find that block");
							HTTPConnection* httpconnecion = new HTTPConnection(conn_socket, server_block);
							kq.addEvent(conn_socket, EVFILT_READ, httpconnecion);
							timer.init_time(conn_socket);
						}
						// HTTPConnection 처리
						else if (dynamic_cast<HTTPConnection*>(udata) != NULL) {
							HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);
							int result = hc->run();
							std::cout << "result : " << result << std::endl;
							std::cout << "time : " << timer.get_time(kq.getFdByEventIndex(i)) << std::endl;
						
							if (kq.isCloseByEventIndex(i)) {
								//int fd = kq.getFdByEventIndex(i);
								timer.del_time(kq.getFdByEventIndex(i));
								delete hc;
							} else if (result == HTTPConnection::RESPONSE) {
								// READ -> WRITE
								int fd = kq.getFdByEventIndex(i);
								std::cout << "kq(r) : " << fd << std::endl;
								kq.disableEvent(fd, EVFILT_READ, udata);
								kq.enableEvent(fd, EVFILT_WRITE, udata);
								timer.clean_time(fd);
							} else if (result == HTTPConnection::CLOSE) {
								// 이벤트 제거
								int fd = kq.getFdByEventIndex(i);
								std::cout << "kq(w) : " << fd << std::endl;
								kq.removeEvent(fd, EVFILT_WRITE, udata);
								timer.del_time(fd);
							} else {
								// kevent를 한번만 사용하기 때문에 이전 Write가 한번 더 들어와서 이 때 close 처리를 해야 함.
								timer.del_time(kq.getFdByEventIndex(i));
								delete hc;
							}
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
