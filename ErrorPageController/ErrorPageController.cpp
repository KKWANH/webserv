#include "ErrorPageController.hpp"

std::string ErrorPageController::getErrorBody(std::string _error_code) {
	std::stringstream rtn;

	rtn << "<!DOCTYPE html>\n";
	rtn << "<html>\n";
	rtn << "\t<head>\n";
	rtn << "\t\t<link rel=\"icon\" type=\"image/x-icon\" href=\"assets/favicon.ico\" />\n";
	rtn << "\t\t<title>" + _error_code + " " + _status.getStatus(_error_code) + "</title>\n";
	rtn << "\t</head>\n";
	rtn << "\t<body>\n";
	rtn << "\t\t<center><h1>" + _error_code + " " + _status.getStatus(_error_code) + "</h1>";
	rtn << "</center>\n";
	rtn << "\t\t<hr>\n";
	rtn << "\t\t<center>nginx/1.21.6</center>\n";
	rtn << "\t</body>\n";
	rtn << "</html>";
	return (rtn.str());
}