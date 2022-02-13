# include				"include/Utils.hpp"
# include				"include/Config_Controller.hpp"
# include				"include/Socket_Controller.hpp"
# include				"include/Server_Process.hpp"
# include				"include/HTTPMsg_Controller.hpp"
# include				"include/KQueue_Controller.hpp"

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
	if (_mime.setContent("./config/mime.types") == ERROR)
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