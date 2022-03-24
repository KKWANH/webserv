#include "AutoindexController.hpp"

int
	AutoindexController::isCorrectDir(
		std::string	root_path,
		std::string dir_path,
		std::string index_file)
{
	DIR*
		dir;
	struct dirent
		*diread;
	std::string	
		absolutePath = root_path + dir_path;
	int	
		count = -1;

	// 404 Not Found + extension html
	if ((dir = opendir(absolutePath.c_str())) == NULL)
		return (count);
	while ((diread = readdir(dir)) != NULL)
	{
		if (diread->d_ino == 0)
			continue;
		// 200 OK with index file
		if (strcmp(index_file.c_str(), diread->d_name) == 0) {
			closedir(dir);
			return (0);		
		}
		++count;
	}
	closedir(dir);
	return (count);
}

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
	_msg_body << "<title>Index of " + _path + "</title></head>";
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
		_msg_body << "<a href=\"" + _file_name + "\">";
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