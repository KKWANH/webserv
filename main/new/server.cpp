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
	_mime(std::string(MIME_PATH));
extern MimeConfig
	_mime;

int
	main(
		void)
	{
		try
		{
			ServerProcess::serverProcess();
		}
		catch (const std::exception& err)
		{
			std::cerr << err.what() << std::endl;
			return (-1);
		}
		return (0);
}
