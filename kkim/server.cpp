# include				"srcs/utils/Utils.hpp"
# include				"srcs/parse/Config_Controller.hpp"
# include				"srcs/socket/Socket_Controller.hpp"
# include				"srcs/server_process/Server_Process.hpp"
# include				"srcs/http_message/HTTPMsg_Controller.hpp"
# include				"srcs/kqueue/KQueue_Controller.hpp"
# include				"srcs/error/Error_Handler.hpp"
# include				"srcs/parse/ConfigBlocks.hpp"

# define				CONF_PATH				"./conf/nginx.conf"

ConfigController		_config;
extern ConfigController	_config;
ConfigController		_mime;
extern ConfigController	_mime;

int
	main(int _arc, char **_arv)

{
	const char* _conf_path = static_cast<const char *>(CONF_PATH);
	NginxConfig::GlobalConfig
		nginx_config(_conf_path);
	std::cout << "HERE!!!!!\n" <<
		nginx_config._http._dir_map["charset"] << "\n" <<
		std::endl;
	// Ngi

	_config.setIsMIME(false);

	SocketController	_socket;

	try
	{
		// Set config file
		if (_arc == 1)
			_config.setContent("./settings/wsv.config");
		else
			_config.setContent(_arv[1]);

		// Set MIME types
		_mime.setIsMIME(true);
		_mime.setContent("./settings/mime.types");

		// Init socket communication
		_socket.init();
		
		// Init polling
		KQueueController
			_kqueue;
		_kqueue.init(_socket.getSocketServer());

		// Non-blocking socket communication
		ServerProcess::serverProcess(&_socket, &_kqueue);
	}
	catch (const std::exception& _err)
	{
		std::cerr << _err.what() << std::endl;
		close(_socket.getSocketServer());
		return (ERROR);
	}

	close(_socket.getSocketServer());
	return				(0);
}