#include "ErrorPageController.hpp"

std::string
	ErrorPageController::getErrorBody(
		std::string _error_code)
{
	std::stringstream
		_msg_body;

	_msg_body << "<!DOCTYPE html>\n";
	_msg_body << "<html>\n";
		_msg_body << "\t<head>\n";
			_msg_body << "\t\t<link rel=\"icon\" type=\"image/x-icon\" href=\"assets/favicon.ico\" />\n";
			_msg_body << "\t\t<title>" +
							_error_code + " " +
							_status.getStatus(_error_code) +
							"</title>\n";
		_msg_body << "\t</head>\n";
		_msg_body << "\t<body>\n";
			_msg_body << "\t\t<center><h1>" +
						 _error_code + " " +
						 _status.getStatus(_error_code) +
						 "</h1>";
			_msg_body << "</center>\n";
			_msg_body << "\t\t<hr>\n";
			_msg_body << "\t\t<center>nginx/1.21.6</center>\n";
		_msg_body << "\t</body>\n";
	_msg_body << "</html>";
	return (_msg_body.str());
}