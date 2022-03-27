#ifndef ERRORPAGECONTROLLER_HPP
# define ERRORPAGECONTROLLER_HPP

#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <fstream>
#include "FileController.hpp"
#include "ConfigStatus.hpp"
#include "ConfigBlocks.hpp"
#include "HTTPData.hpp"

extern StatusConfig
	_status;
extern NginxConfig::GlobalConfig
	_config;

class ErrorPageController {
public:
	ErrorPageController(
		HTTPData *data);
	
	std::string
		getDefaultErrorBody(
			std::string _error_code);

	std::vector<std::string>
		findLocation(
			std::string uri);

	std::string
		findRoot(
			void);

	std::string
		findErrorPage(
			std::string _error_code);

	private:
		HTTPData	*data;
};

#endif