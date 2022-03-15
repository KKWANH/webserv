#include "FileController.hpp"

int			FileController::getFileSize(std::string path) {
	int size = 0;
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
	
    if (ifs.is_open() == 0) {
        std::cout << "NO FILE" << std::endl;
		return (-1);
	}
    ifs.seekg(0, std::ios::end);
    size = ifs.tellg();
    ifs.close();
	return (size);
}