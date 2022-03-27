#include "ErrorPageController.hpp"

ErrorPageController::ErrorPageController(
		void) { }

void
	ErrorPageController::setHTTPData(
		HTTPData *_arg_data) {
	this->_data = _arg_data;
}

std::string
	ErrorPageController::getDefaultErrorBody(
		std::string _error_code)
{
	std::stringstream
		_msg_body;

	_msg_body << "<!DOCTYPE html>\n";
	_msg_body << "<html>\n";
		_msg_body << "\t<head>\n";
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

std::vector<std::string>
	ErrorPageController::findLocation(
		std::string _uri)
{
	std::cout << "findLocation uri : ";
	std::vector<NginxConfig::LocationBlock>
		_location = _config._http._server[
			this->_data->server_block]._location;
	std::cout << "findLocation uri : " << this->_data->server_block << std::endl;
	std::vector<NginxConfig::LocationBlock>::iterator
		_itr = _location.begin();
	std::cout << "findLocation uri : " << this->_data->server_block << std::endl;
	for (_itr = _location.begin();
		 _itr != _location.end();
		 _itr++)
		if (_itr->_location.compare(_uri) == 0)
			return (_itr->_error_page);
	std::cout << "findLocation uri : " << this->_data->server_block << std::endl;
	return (_config._http._server[
		this->_data->server_block]._error_page);
}

std::string
	ErrorPageController::findRoot(
		void)
{
	std::map<std::string, std::string>::iterator
		_root_finder = _config._http._server[this->_data->server_block]._dir_map.find("root");

	if (_root_finder != _config._http._server[
		this->_data->server_block]._dir_map.end())
		return (_root_finder->second);
	else
		return ("./static_html");
}

std::string
	ErrorPageController::findErrorPage(
		std::string _error_code)
{
	std::cout << "check 1\n";
	std::cout << "check 2" << this->_data->is_autoindex << std::endl;
	std::vector<std::string>
		_error_page = findLocation(this->_data->url_directory);
	std::cout << "check 3\n";
	int
		_idx = 0;
	std::string
		_root = findRoot();
	
	for (_idx = 0; _idx < (int)_error_page.size(); _idx++)
		if (_error_page[_idx] == _error_code)
		{
			std::string
				_file_name = _root + "/" + _error_page[_error_page.size() - 1];
			_file_name = FileController::toAbsPath(_file_name);
			if (access(_file_name.c_str(), F_OK) == 0)
			{
				std::ifstream
					_file(_file_name.c_str());
				std::stringstream
					_buffer;
				_buffer << _file.rdbuf();
				return (_buffer.str());
			}
			break;
		}
	return (getDefaultErrorBody(_error_code));
}