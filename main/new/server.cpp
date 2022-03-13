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
	_config(static_cast<const char *>(CONF_PATH));
extern NginxConfig::GlobalConfig
	_config;

MimeConfig
	_mime;
extern MimeConfig
	_mime;

int
	main(int _arc, char** _arv)
	{
		(void)_arc;
		(void)_arv;
		try
		{
			if (_arc == 2)
			{
				_mime.setUri(std::string(MIME_PATH));
				
			}
			if (_arc == 3)
			{
				std::string
					_arv2 = std::string(_arv[2]);
				// if (_arv2.substr(_arv2.length() - 4, _arv2.length());
			}
			_mime.setMIME();
			ServerProcess::serverProcess();
		}
		catch (const std::exception& err)
		{
			std::cerr << err.what() << std::endl;
			return (-1);
		}

		return (0);
}
