# include	"ConfigController.hpp"
# include	"SocketController.hpp"
# include	"ServerProcess.hpp"
# include	"KernelQueueController.hpp"
# include	"MIMEController.hpp"
# include	"ErrorHandler.hpp"
# include	"ConfigBlocks.hpp"

# include	<iostream>
# include	<cstring>
# define	CONF_PATH	"./setting/nginx.conf"

MIMEController
	mime;
extern MIMEController
	mime;
NginxConfig::GlobalConfig
	_config(static_cast<const char *>(CONF_PATH));
extern NginxConfig::GlobalConfig
	_config;

int
	main(
		void)
	{
		try
		{
			mime.setMIME();
			ServerProcess::serverProcess();
		}
		catch (const std::exception& err)
		{
			std::cerr << err.what() << std::endl;
			return (-1);
		}
		return (0);
}
