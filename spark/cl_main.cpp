#include <iostream>
#include "ListeningSocket.hpp"
#include "ConnectionSocket.hpp"
#include "KernelQueue.hpp"
#include "Timer.hpp"

#define CONF_PATH "./conf/nginx.cong"

#ifndef WEBSERV_VERSION
#define WEBSERV_VERSION "0.0.0"
#endif

int main(int ac, char *av[])
{
    Timer timer;
    KernelQueue kq(1.0);
    const char* confPath = static_cast<const char *>(CONF_PATH);
    if (argc == 2) {
        confPath = argv[1];
    } else if (argc > 2) {
        std::cerr << "\033[0;31m[err] Argument Count is Not Good \033[0m" << std::endl;
        return (1);
    }

     try {
        NginxConfig::GlobalConfig nginxConfig(confPath);
        for (std::size_t i = 0; i < nginxConfig._http.server.size(); i++) {
            ListeningSocket* lSocket = new ListeningSocket(nginxConfig._http.server[i], 60000);
            if (lSocket->runSocket())
                return (1);
            kq.addReadEvent(lSocket->getSocket(), reinterpret_cast<void*>(lSocket));
        }

        while (true) {
            int result = kq.getEventsIndex();
            if (result == 0) {
                std::cout << "\x0dwaiting..." << std::flush;
            } else {
                for (int i = 0; i < result; ++i) {
                    Socket* instance = reinterpret_cast<Socket*>(kq.getInstance(i));
                    long data = kq.getData(i);
                    if (dynamic_cast<KernelQueue::PairQueue*>(instance) != NULL) {
                        KernelQueue::PairQueue* pairQueue = reinterpret_cast<KernelQueue::PairQueue*>(instance);
                        pairQueue->stopSlave();
                    } else if (dynamic_cast<ListeningSocket*>(instance) != NULL) {
                        for (long i = 0; i < data; i++) {
                            ConnectionSocket* cSocket = new ConnectionSocket(instance->getSocket(), instance->getConfig(), nginxConfig);
                            timer.addObj(cSocket, std::atoi(instance->getConfig().dirMap["keepalive_timeout"].c_str()));
                            kq.addReadEvent(cSocket->getSocket(), reinterpret_cast<void*>(cSocket));
                        }
                    } else if (dynamic_cast<ConnectionSocket*>(instance) != NULL) {
                        ConnectionSocket* cSocket = dynamic_cast<ConnectionSocket*>(instance);
                        cSocket->setDynamicBufferSize(data);
                        if (kq.isClose(i)) {
                            timer.delObj(cSocket, ConnectionSocket::ConnectionSocketKiller);
                        } else if (kq.isReadEvent(i)) {
                            HTTPRequestHandler::Phase phase = cSocket->HTTPRequestProcess();
                            if (phase < HTTPRequestHandler::FINISH) {
                                kq.pairStopMaster(i);
                            }
                            if (phase == HTTPRequestHandler::FINISH) {
                                kq.deletePairEvent(i);
                                kq.modEventToWriteEvent(i);
                            } else if (phase == HTTPRequestHandler::BODY_TYPE_CHECK) {
                                kq.setPairEvent(i, cSocket->getFilefd(), false);
                            }
                        } else if (kq.isWriteEvent(i)) {
                            HTTPResponseHandler::Phase phase = cSocket->HTTPResponseProcess();
                            if (phase < HTTPResponseHandler::FINISH) {
                                kq.pairStopMaster(i);
                            }
                            if (phase == HTTPResponseHandler::CGI_RECV_HEAD_LOOP) {
                                kq.setPairEvent(i, cSocket->getCGIfd(), true);
                            } else if (phase == HTTPResponseHandler::FINISH) {
                                kq.deletePairEvent(i);
                                timer.delObj(cSocket, ConnectionSocket::ConnectionSocketKiller);
                            } else if (phase == HTTPResponseHandler::FINISH_RE) {
                                kq.deletePairEvent(i);
                                kq.modEventToReadEvent(i);
                                timer.resetObj(cSocket, std::atoi(instance->getConfig().dirMap["keepalive_timeout"].c_str()));
                            }
                        }
                    } else {
                        throw ErrorHandler("Error: Event Handling Failure", ErrorHandler::CRITICAL, "main");
                    }
                }
            }
            timer.CheckTimer(ConnectionSocket::ConnectionSocketKiller);
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        return (1);
    }

}