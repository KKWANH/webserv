#ifndef AUTOINDEXCONTROLLER_HPP
# define AUTOINDEXCONTROLLER_HPP

#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "FileController.hpp"

class AutoindexController {
	public:
		static std::string
			getAutoIndexBody(
				std::string root_path, std::string dir_path);
};

#endif