#ifndef FILECONTROLLER_HPP
# define FILECONTROLLER_HPP

#include <cstring>
#include <fstream>
#include <iostream>

class FileController
{
    private:

    public:
		static int		getFileSize(std::string path);
};

#endif