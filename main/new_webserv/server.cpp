# include				"ConfigController.hpp"
# include				"SocketController.hpp"
# include				"ServerProcess.hpp"
# include				"KernelQueueController.hpp"
# include				"MIMEController.hpp"
# include				"ErrorHandler.hpp"
# include				"ConfigBlocks.hpp"

# define				CONF_PATH				"./conf/nginx.conf"

ConfigController		config;
extern ConfigController	config;
MIMEController			mime;
extern MIMEController	mime;
NginxConfig::GlobalConfig
						_config;

int
	main(int argc, char **argv)
{
	SocketController
		Socket;
	const char*
		_conf_path = static_cast<const char *>(CONF_PATH);


	try {
		if (argc != 2)
			_config.startConfig(_conf_path);
		else
			_config.startConfig(argv[1]);
		// config file setting
		config.setConfig(argc, argv);

		// mime types setting
		mime.setMIME();

		// socket communication init
		Socket.init();

		// polling init
		KernelQueueController Kqueue;
		Kqueue.init(Socket.getSocketServer());

		// non-blocking socket communication 
		ServerProcess::serverProcess(&Socket, &Kqueue);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		close(Socket.getSocketServer());
		return (-1);
	}
	close(Socket.getSocketServer());
	return (0);
}
