# include				"include/Utils.hpp"
# include				"include/Config_Controller.hpp"
# include				"include/Socket_Controller.hpp"

ConfigController		_cnf(false);
ConfigController		_mim(true);

int
	main(int _arc, char **_arv)
{
	if (_arc != 2)
		return (-1);
	// Set config file
	if (_cnf.setContent(_arv[1]) == ERROR)
		return			(ERROR);
	// Set MIME types
	if (_mim.setContent("./config/mime.types") == ERROR)
		return			(ERROR);

	// Init socket communication
	SocketController	_sck;
	if (_sck.init() == ERROR)
		return			(ERROR);
	
	// Init polling
}