 #include <iostream>

 #include "./configHandler/Timer.hpp"
 #include "KernelQueue.hpp"
 #include "ListeningSocket.hpp"
 #include "ConnectionSocket.hpp"

 #define CONF_PATH "./conf/skim.conf"

int	main(int ac, char *av[])
{
	Timer		timer;
	KernelQueue	kq(1.0);

	const char	*confPath = CONF_PATH;

	if (ac == 2)
		confPath = av[1];
	else if (ac > 2) {
		std::cerr << "bad Argument" << std::endl;
		return (1);
	}

	try {
		NginxConfig::GlobalConfig	nginxConfig(confPath); // nginx config 파일에서 읽어온 정보 저장하기
		for (int i = 0; i < NginxConfig._http.server.size(); i++) {
			ListeningSocket *lSocket = new ListeningSocket(nginxConfig._http.server[i], 60000);
			if (lSocket->runSocket())
				return (1);
			kq.addReadEvent(lSocket->getSocket(), lSocket);
		}

		while (1) {
			int	result = kq.getEventIndex();
			if (result == 0)
				std::cout << "Waiting..." << std::endl;
			else {
				for (int i = 0; i < result; i++)
				{
					Socket	*tmp = reinterpret_cast<Socket *>(kq.getTmpPtr(i));
					long	data = kq.getData(i);
					if (dynamic_cast<KernelQueue::PairQueue *>(tmp) != NULL) { // 연결 1
						KernelQueue::PairQueue	*pair = reinterpret_cast<KernelQueue::PairQueue *>(tmp);
						pair->stopSlave();
					} else if (dynamic_cast<ListeningSocket *>(tmp) != NULL) { // 연결 2
						for (int i = 0; i < data; i++) {
							ConnectionSocket	*cSocket = new ConnectionSocket(tmp->getSocket(), tmp->getConfig(), nginxConfig);
							timer.addObj(cSocket, std::atoi(tmp->getConfig().dirMap["keepalive_timeout"].c_str()));
							kq.addReadEvent(cSocket->getSocket(), reinterpret_cast<void *>(cSocket));
						}
					} else if (dynamic_cast<ConnectionSocket *>(tmp) != NULL) { // client 연결
						ConnectionSocket* cSocket = dynamic_cast<ConnectionSocket*>(tmp);
						cSocket->setDynamicBufferSize(data);
						if (kq.isClose(i)) {
							timer.delObj(cSocket, ConnectionSocket::ConnectionSocketKiller);
						} else if (kq.isReadEvent(i)) { // read event
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
						} else if (kq.isWriteEvent(i)) { // write event
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
								timer.resetObj(cSocket, std::atoi(tmp->getConfig().dirMap["keepalive_timeout"].c_str()));
							}
						}
					} else {
						throw std::string("bad event");
					}
				}
			}
			timer.checkTime(ConnectionSocket::ConnectionSocketKiller);
		}
	} catch (const std::exception &error) {
		std::cerr << error.what() << std::endl;
		return (1);
	}
	return (0);
}
