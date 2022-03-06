# include "./ConfigBlocks.hpp"


# define				CONF_PATH				"./conf/nginx.conf"

int main(void)
{
	const char* _conf_path = static_cast<const char *>(CONF_PATH);
	NginxConfig::GlobalConfig
		nginx_config(_conf_path);
	std::cout << "HERE!!!!!\n" <<
		nginx_config._http._server[1]._location[1]._return[0] << std::endl <<
		nginx_config._http._server[1]._location[1]._return.empty() <<std::endl <<
		nginx_config._http._server[1]._location[2]._return.empty() <<std::endl <<
		// nginx_config._http._server[1]._location[2]._return[0] <<std::endl <<
		// nginx_config._http._server[1]._location[2]._return[1] <<std::endl <<
		// "http : charset" << nginx_config._http._dir_map["charset"] << "\n" <<
		// "worker_process : " << nginx_config._none._user << "\n" <<
		// "user : " << nginx_config._none._worker_prc << "\n" << 
		// "http : server 0 : listen : " << nginx_config._http._server[0]._dir_map["listen"] << "\n" <<
		// "http : server 0 : root : " << nginx_config._http._server[0]._dir_map["root"] << "\n" <<
		// "http : server 0 : location 0 : location" << nginx_config._http._server[0]._location[0]._location << "\n" <<
		// "http : server 1 : listen : " << nginx_config._http._server[1]._dir_map["listen"] << "\n" <<
		// "http : server 1 : keepalive_timeout : " << nginx_config._http._server[1]._dir_map["keepalive_timeout"] << "\n" <<
		// "http : server 1 : autoindex : " << nginx_config._http._server[1]._dir_map["autoindex"] << "\n" <<
		std::endl;
	return (0);
}