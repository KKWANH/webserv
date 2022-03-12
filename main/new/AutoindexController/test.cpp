#include <cstring>
#include <iostream>
#include "./AutoindexController.hpp"

int main(void) {
	std::string root_path = "/Users/server/Desktop/BHS/webserv/hybae/local/static_html";
    std::string dir_path = "/js";
	std::string index_file = "index.html";
	if (AutoindexController::isCorrectDir(root_path, dir_path, index_file) > 0)
        std::cout << AutoindexController::getAutoIndexBody(root_path, dir_path) << std::endl;
	return (0);
}