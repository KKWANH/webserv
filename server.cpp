#include	"ConfigBlocks.hpp"
#include	"ConfigMime.hpp"
#include	"ConfigStatus.hpp"
#include	"SocketController.hpp"
#include	"ServerProcess.hpp"
#include	"KernelQueueController.hpp"
#include	"ErrorHandler.hpp"
#include	"ErrorPageController.hpp"

#include	<iostream>
#include	<cstring>

#define	CONF_PATH	"./setting/nginx.conf"
#define	MIME_PATH	"./setting/mime.types"
#define	STAT_PATH	"./setting/status_code.txt"

NginxConfig::GlobalConfig			_config;
extern NginxConfig::GlobalConfig	_config;

MimeConfig							_mime;
extern MimeConfig					_mime;

StatusConfig						_status;
extern StatusConfig					_status;

ErrorPageController					_error_page;

void	check_argv_str (std::string& _conf_uri, std::string  _tmp) {
	if (_tmp.substr(_tmp.length() - 5, _tmp.length()).compare(".conf") == 0)
		_conf_uri = _tmp;
	else
		throw ErrorHandler(__FILE__, __func__, __LINE__, std::string("Invalid file name on argument(.conf, .types and .txt only) : " + _tmp));
}

int		main(int argc, char** argv) {
	try {
		std::string
			_conf_uri = CONF_PATH,
			_mime_uri = MIME_PATH,
			_stat_uri = STAT_PATH;
		if (argc == 2)
			check_argv_str(_conf_uri, argv[1]);
		else if (argc != 1)
			throw ErrorHandler(__FILE__, __func__, __LINE__, "Too many arguments. Please input one ~ three, or just nothing.");
		_config.startConfig(_conf_uri);
		_mime.startConfig(_mime_uri);
		_status.startConfig(_stat_uri);
		ServerProcess::serverProcess();
	}
	catch (const ErrorHandler& err) {
		std::cerr << err.what() << std::endl;
		return (-1);
	}
	return (0);
}
