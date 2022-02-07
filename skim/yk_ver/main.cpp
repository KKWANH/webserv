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
					if (dynamic_cast<KernelQueue::PairQueue *>(tmp) != NULL) { // ??
						KernelQueue::PairQueue	*pair = reinterpret_cast<KernelQueue::PairQueue *>(tmp);
						pair->stopSlave();
					} else if (dynamic_cast<ListeningSocket *>(tmp) != NULL) {
						for (int i = 0; i < data; i++) {
							ConnectionSocket	*cSocket = new ConnectionSocket(instance->getSocket(), instance->getConfig(), nginxConfig);
							timer.addObj(cSocket, std::atoi(instance->getConfig().dirMap["keepalive_timeout"].c_str()));
							kq.addReadEvent(cSocket->getSocket(), reinterpret_cast<void *>(cSocket));
						}
					} else if (dynamic_cast<ConnectionSocket *>(tmp) != NULL) {

					}
				}
			}
		}
	} catch (const std::exception &error) {
		std::cerr << error.what() << std::endl;
		return (1);
	}
}
