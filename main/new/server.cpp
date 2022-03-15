# include	"ConfigBlocks.hpp"
# include	"ConfigMime.hpp"
# include	"SocketController.hpp"
# include	"ServerProcess.hpp"
# include	"KernelQueueController.hpp"
# include	"ErrorHandler.hpp"

# include	<iostream>
# include	<cstring>

# define	CONF_PATH	"./setting/nginx.conf"
# define	MIME_PATH	"./setting/mime.types"

NginxConfig::GlobalConfig
	_config;
extern NginxConfig::GlobalConfig
	_config;

MimeConfig
	_mime;
extern MimeConfig
	_mime;
// 

void
	check_argv_str(
		std::string& _conf_uri,
		std::string& _mime_uri,
		std::string  _tmp)
{
	if (_tmp.substr(_tmp.length() - 5, _tmp.length()).compare(".conf") == 0)
		_conf_uri = _tmp;
	else if (_tmp.substr(_tmp.length() - 6, _tmp.length()).compare(".types") == 0)
		_mime_uri = _tmp;
	else
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"Invalid file name on argument(.conf and .types only) : " + _tmp);
}

int
	main(int _arc, char** _arv)
	{
		try
		{
			std::string
				_conf_uri = CONF_PATH,
				_mime_uri = MIME_PATH;
			if (_arc == 2)
				check_argv_str(_conf_uri, _mime_uri, _arv[1]);
			else if (_arc == 3)
				check_argv_str(_conf_uri, _mime_uri, _arv[2]);
			else if (_arc != 1)
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"Too many arguments. Please input one, two, or nothing.");
			_config.startConfig(_conf_uri);
			_mime.startConfig(_mime_uri);ServerProcess::serverProcess();
		}
		catch (const std::exception& err)
		{
			std::cerr << err.what() << std::endl;
			return (-1);
		}

		return (0);
}
