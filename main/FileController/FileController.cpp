# include	"FileController.hpp"

int
	FileController::getFileSize(
		std::string _pth)
{
	int
		_siz = 0;
    std::ifstream
		_ifs(_pth, std::ios::in | std::ios::binary);

	// @TODO : errorhandler non_crit으로 바꿔야하나?
    if (_ifs.is_open() == 0)
	{
        std::cout << "NO FILE" << std::endl;
		return (-1);
	}
    _ifs.seekg(0, std::ios::end);
    _siz = _ifs.tellg();
    _ifs.close();
	return (_siz);
}

FileController::Type
	FileController::modeToType(
		mode_t _mod)
{
	if (S_ISREG(_mod))
		return (FIL);
	else if (S_ISDIR(_mod))
		return (DIR);
	else
		return (NON);
}

FileController::Type
	FileController::checkType(
		std::string _pth)
{
	struct stat
		_stat;
	if (stat(_pth.c_str(), &_stat) == -1)
		return (NON);
	return (modeToType(_stat.st_mode));
}

std::string&
	FileController::toAbsPath(
		std::string& _pth)
{
	if (_pth.empty() == false)
		if (_pth.at(0) != '/')
		{
			char*
				_pwd = getcwd(NULL, 0);
			_pth = std::string(_pwd) + "/" + _pth;
			delete _pwd;
		}
	return (_pth);
}