# include				"srcs/utils/Utils.hpp"
# include				"srcs/parse/Config_Controller.hpp"
# include				"srcs/socket/Socket_Controller.hpp"
# include				"srcs/server_process/Server_Process.hpp"
# include				"srcs/http_message/HTTPMsg_Controller.hpp"
# include				"srcs/kqueue/KQueue_Controller.hpp"
# include				"srcs/error/Error_Handler.hpp"

ConfigController		_config;
extern ConfigController	_config;
ConfigController		_mime;
extern ConfigController	_mime;

int
	main(int _arc, char **_arv)

{
	_config.setIsMIME(false);

	SocketController	_socket;

	try
	{
		if (_arc == 1)
		{
			if (_config.setContent("./settings/wsv.config") == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "config file setting error");
		}
		else if (_arc > 2)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "too many arguments");
		else
		{
			if (_config.setContent(_arv[1]) == ERROR)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "config file setting error");
		}

		// Set MIME types
		_mime.setIsMIME(true);
		if (_mime.setContent("./settings/mime.types") == ERROR)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "mime file setting error");

		// Init socket communication
		if (_socket.init() == ERROR)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "socket communication init error");
		
		// Init polling
		KQueueController	_kqueue;
		if (_kqueue.init(_socket.getSocketServer()) == ERROR)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "polling init error");

		// Non-blocking socket communication
		if (ServerProcess::serverProcess(&_socket, &_kqueue) == ERROR)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "non-blocking socket communication error");
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