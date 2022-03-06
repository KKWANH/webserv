#include "ConfigController.hpp"
#include "SocketController.hpp"
#include "ServerProcess.hpp"
#include "KernelQueueController.hpp"
#include "MIMEController.hpp"
#include "ErrorHandler.hpp"
#include "ConfigBlocks.hpp"

#include <iostream>
#include <cstring>
# define CONF_PATH "./conf/nginx.conf"


ConfigController config;
extern ConfigController config;
MIMEController mime;
extern MIMEController mime;
NginxConfig::GlobalConfig _config(static_cast<const char *>(CONF_PATH));
extern NginxConfig::GlobalConfig _config;

int main(void) {
	// const char*
		// _conf_path = static_cast<const char *>(CONF_PATH);
	std::cout << "portportport : " << _config._http._server[0]._dir_map["listen"] << std::endl;
	try {
		// // config file setting
		// config.setConfig(argc, argv);
		// if (argc != 2)
		// 	_config.startConfig(_conf_path);
		// else
		// 	_config.startConfig(argv[1]);
		// config file settingㄱ
		// config.setConfig(argc, argv);

		// mime types setting
		mime.setMIME();

		// non-blocking socket communication 
		ServerProcess::serverProcess();
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		return (-1);
	}
	return (0);
}
