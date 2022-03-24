# include	"FileController.hpp"

FileController::FileController(
		void)
{ };

FileController::FileController(
		std::string _tmp_path,
		Mode		_tmp_mode)
{
	if (_tmp_path.empty() ||
		setPath(_tmp_path) == -1)
	{
		this->_path = std::string("");
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"something wrong about your path.", ErrorHandler::CRIT);
	}
	this->_mode = _tmp_mode;
	if (this->_mode == READ)
	{
		this->_type = checkType(_path);
		if (_type == FileController::FILE)
		{
			_meta = getMetaData(_path);
			_fd = open(_path.c_str(), O_RDONLY);
		}
		else if (_type == FileController::DIRECTORY)
		{
			_meta = getMetaData(_path);
			getFilesOfFolder(_path, _files_meta);
		}
		else
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"file not found.", ErrorHandler::CRIT);
	}
	else
	{
		_type = FileController::FILE;
		_fd = open(_path.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
	}
};

FileController::~FileController(
		void)
{
	delete _meta;
	std::vector<FileMetaData*>::iterator
		_itr;
	for (_itr = _files_meta.begin(); _itr != _files_meta.end(); ++_itr)
		delete *(_itr);
	if (_fd != -1)
		close(_fd);
}

int
	FileController::setPath(
		std::string _tmp_path)
{
	this->_path = _tmp_path;
	this->_path = toAbsPath(this->_path);
	if (checkType(this->_path) == FileController::NON)
		return (-1);
	return (0);
}

std::string
	FileController::getPath(
		void)
{
	return (this->_path);
}

FileController::FileMetaData*
	FileController::getMetaData(
		std::string _tmp_path)
{
	char
		_tim_buffer[20];
	FileController::FileMetaData*
		_rst = NULL;
	struct stat
		_buf;
	struct passwd*
		_u_data;
	struct tm*
		_tim_info;
	if (stat(_tmp_path.c_str(), &_buf) == -1 ||
		!(	S_ISREG(_buf.st_mode) ||
			S_ISDIR(_buf.st_mode)))
		return (NULL);

	_rst = new FileController::FileMetaData();
	_u_data = getpwuid(_buf.st_uid);
	_tim_info = std::localtime(&(_buf.st_mtimespec.tv_sec));
	std::strftime(_tim_buffer, 20, "%Y/%m/%d %H:%M:%S", _tim_info);

	size_t
		_find = _tmp_path.rfind("/");
	if (_find == std::string::npos)
		_rst->_name = _tmp_path;
	else
		_rst->_name = _tmp_path.substr(_find + 1);
	if (_u_data != NULL)
		_rst->_user = std::string(_u_data->pw_name);
	else
		_rst->_user = std::string("");
	_rst->_type = modeToType(_buf.st_mode);
	_rst->_size = _buf.st_size;
	_rst->_genr_time = std::string(_tim_buffer);

	return (_rst);
}

void
	FileController::getFilesOfFolder(
		std::string&				_tmp_path,
		std::vector<FileMetaData*>& _tmp_files_meta)
{
	struct dirent*
		_dirent;
	DIR* _dir = opendir(_tmp_path.c_str());
	while ((_dirent = readdir(_dir)) != NULL)
	{
		std::string
			_sub_path;
		if (_tmp_path.at(_tmp_path.length() - 1) == '/')
			_sub_path = _tmp_path + std::string(_dirent->d_name, _dirent->d_namlen);
		else
			_sub_path = _tmp_path + "/" + std::string(_dirent->d_name, _dirent->d_namlen);
		_tmp_files_meta.push_back(getMetaData(_sub_path));
	}
	closedir(_dir);
}

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

int
	FileController::getFilesSize(
		void) const
{
	if (this->_mode == READ)
		return (this->_files_meta.size());
	else
		return (-1);
}

FileController::FileMetaData*
	FileController::getFiles(
		int _idx) const
{
	if (_idx < 0)	
		return (_meta);
	else if (_files_meta.size() <= size_t(_idx))
		return (NULL);
	else
		return (_files_meta[_idx]);
}

FileController::Type
	FileController::modeToType(
		mode_t _mod)
{
	if (S_ISREG(_mod))
		return (FILE);
	else if (S_ISDIR(_mod))
		return (DIRECTORY);
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

std::string
	FileController::getPrePath(
		std::string _pth)
{
	std::string
		_tmp;
	if (_pth.substr(_pth.length() - 1, _pth.length()) == "/")
		_tmp = _pth.substr(0, _pth.length() - 1);
	int	
		_idx = _tmp.find_last_of("/");
    return (_tmp.substr(0, _idx + 1));
}