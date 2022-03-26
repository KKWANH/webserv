// NOTE: modified coding convention by joopark
#ifndef ERRORPAGECONTROLLER_HPP
# define ERRORPAGECONTROLLER_HPP

#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "FileController.hpp"
#include "ConfigStatus.hpp"

extern StatusConfig _status;

class ErrorPageController {
public:
	static std::string getErrorBody(std::string _error_code);
};

#endif