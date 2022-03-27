#ifndef ERRORPAGECONTROLLER_HPP
# define ERRORPAGECONTROLLER_HPP

# include	<dirent.h>
# include	<cstring>
# include	<iostream>
# include	<sstream>
# include	<iomanip>
# include	<vector>

# include	"FileController.hpp"
# include	"ConfigBlocks.hpp"
# include	"ConfigStatus.hpp"
# include	"HTTPData.hpp"

extern StatusConfig
	_status;
extern NginxConfig::GlobalConfig
	_config;

class ErrorPageController
{
	public:
		HTTPData*
			_data;
	public:
		ErrorPageController(
				void);
		
		void
			setHTTPData(
				HTTPData *_arg_data);

		static std::string
			getDefaultErrorBody(
				std::string _error_code);

		std::vector<std::string>
			findLocation(
				std::string _uri);
		
		std::string
			findRoot(
				void);
		
		std::string
			findErrorPage(
				std::string _error_code);
};

#endif