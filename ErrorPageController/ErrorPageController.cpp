#include "ErrorPageController.hpp"

ErrorPageController::ErrorPageController(HTTPData *_data) : data(_data) {}

std::vector<std::string>
	ErrorPageController::findLocation(
		std::string uri)
{
	std::vector<NginxConfig::LocationBlock> location = _config._http._server[data->server_block]._location;
	std::vector<NginxConfig::LocationBlock>::iterator it = location.begin();

	for (it = location.begin(); it != location.end(); it++) {
		if (it->_location.compare(uri) == 0) {
			return (it->_error_page);
		}
	}
	return (_config._http._server[data->server_block]._error_page);
}

std::string
	ErrorPageController::findRoot(
		void)
{
	std::map<std::string, std::string>::iterator	rootFinder;

	rootFinder = _config._http._server[data->server_block]._dir_map.find("root");

	if (rootFinder != _config._http._server[data->server_block]._dir_map.end())
		return (rootFinder->second);
	else
		return ("./static_html");
}

std::string
	ErrorPageController::findErrorPage(
		std::string _error_code)
{
	std::vector<std::string> error_page = findLocation(data->uri_dir);
	std::vector<std::string>::iterator pagePos = find(error_page.begin(), error_page.end(), _error_code);
	std::string	root = findRoot();

	for (; pagePos != error_page.end(); pagePos++) {
		int finder = (*pagePos).find_last_of(".html");
		if (finder != -1 && access(((root + data->uri_dir + *pagePos).c_str()), F_OK) == 0) {
			std::ifstream file((root + data->uri_dir + *pagePos).c_str());
			std::stringstream buffer;
			buffer << file.rdbuf();
			return (buffer.str());
		}
	}
	return ("NULL");
}

std::string
	ErrorPageController::getDefaultErrorBody(std::string _error_code)
{
	std::string ret = findErrorPage(_error_code);

	if (ret != "NULL")
		return (ret);

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
