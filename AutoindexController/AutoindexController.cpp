#include "AutoindexController.hpp"

std::string
	AutoindexController::getAutoIndexBody(
		std::string _root,
		std::string _path)
{
	FileController
		_folder = FileController(_root + _path, FileController::READ);
	std::stringstream
		_msg_body;

	_msg_body << "<!DOCTYPE html><html><head>";
	_msg_body << "<title>Index of " + _path + "</title><link rel=\"icon\" type=\"image/x-icon\" href=\"assets/favicon.ico\" /></head>";
	_msg_body << "<body>";
	_msg_body << "<h1>Index of " + _path + "</h1><hr><pre>";
	for (int _idx = 0; _idx < _folder.getFilesSize(); _idx++)
	{
		if (_folder.getFiles(_idx)->_name == std::string("."))
			continue ;
		std::string
			_file_name;
		if (_folder.getFiles(_idx)->_type == FileController::DIRECTORY)
			_file_name = _folder.getFiles(_idx)->_name + std::string("/");
		else
			_file_name = _folder.getFiles(_idx)->_name;
		std::cout << "Autoindex test : " << _file_name << std::endl;
		_msg_body << "<a href=\"" + _file_name + "\">";
//		_msg_body << "<a href=\"localhost:";
//		_msg_body << _config._http._server[1]._dir_map["listen"];
//		_msg_body << "/";
//		_msg_body << _file_name;
//		_msg_body << "\">";
		_msg_body << std::setw(53) << std::setfill(' ');
		_msg_body << std::left << (_file_name + std::string("</a>"));
		_msg_body << std::right;
		_msg_body << _folder.getFiles(_idx)->_genr_time;
		_msg_body << std::setw(20) << std::setfill(' ');
		if (_folder.getFiles(_idx)->_type == FileController::DIRECTORY)
			_msg_body << "-" << std::endl;
		else
			_msg_body << _folder.getFiles(_idx)->_size << std::endl;
	}
	_msg_body << "</pre><hr></body></html>";

	return (_msg_body.str());
}
