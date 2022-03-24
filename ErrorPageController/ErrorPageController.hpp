#ifndef ERRORPAGECONTROLLER_HPP
# define ERRORPAGECONTROLLER_HPP

#include <dirent.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "FileController.hpp"
#include "ConfigStatus.hpp"

class ErrorPageController {
	public:
    /*
        해당 디렉토리가 존재하지 않는 경우 -1 반환
        해당 디렉토리 내 index 존재할 경우 0 반환
        해당 디렉토리 내 index 존재하지 않는 경우, autoindex로 표시할 파일 및 폴더 수 반환
    */
	static std::string
		getErrorBody(
			std::string _error_code);
};

#endif