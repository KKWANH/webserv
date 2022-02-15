# include				"srcs/utils/Utils.hpp"
# include				"srcs/parse/Config_Controller.hpp"
# include				"srcs/socket/Socket_Controller.hpp"
# include				"srcs/server_process/Server_Process.hpp"
# include				"srcs/http_message/HTTPMsg_Controller.hpp"
# include				"srcs/kqueue/KQueue_Controller.hpp"

ConfigController		_config;
extern ConfigController	_config;
ConfigController		_mime;
extern ConfigController	_mime;

int
	main(int _arc, char **_arv)
{
	_config.setIsMIME(false);
	_mime.setIsMIME(true);
	if (_arc != 2)
		return (-1);
	// Set config file
	if (_config.setContent(_arv[1]) == ERROR)
		return			(ERROR);
	// Set MIME types
	if (_mime.setContent("./settings/mime.types") == ERROR)
		return			(ERROR);

	// Init socket communication
	SocketController	_sck;
	if (_sck.init() == ERROR)
		return			(ERROR);
	
	// Init polling
	KQueueController	_kqu;
	if (_kqu.init(_sck.getSocketServer()) == ERROR)
		return			(ERROR);

	// Non-blocking socket communication
	if (ServerProcess::serverProcess(&_sck, &_kqu) == ERROR)
		return			(ERROR);
	
	close(_sck.getSocketServer());
	return				(0);
}