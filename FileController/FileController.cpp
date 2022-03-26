// NOTE: modified coding convention by joopark
# include	"FileController.hpp"

FileController::FileController(void) : fd(-1) { };

FileController::FileController(std::string path, Mode mode) {
	if (path.empty() || setPath(path) == -1) {
		this->path = std::string("");
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"something wrong about your path.", ErrorHandler::CRIT);
	}
	this->mode = mode;
	if (this->mode == READ) {
		this->type = checkType(path);
		if (this->type == FileController::FILE) {
			this->meta = getMetaData(path);
			this->fd = open(path.c_str(), O_RDONLY);
		} else if (type == FileController::DIRECTORY) {
			this->meta = getMetaData(path);
			getFilesOfFolder(path, this->files_meta);
		} else
			throw ErrorHandler(__FILE__, __func__, __LINE__, "file not found.", ErrorHandler::CRIT);
	} else {
		this->type = FileController::FILE;
		this->fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
	}
};

FileController::~FileController(void) {
	delete this->meta;
	std::vector<FileMetaData*>::iterator iter;
	for (iter = this->files_meta.begin(); iter != this->files_meta.end(); ++iter)
		delete *(iter);
	if (this->fd != -1)
		close(this->fd);
}

int FileController::setPath(std::string path) {
	this->path = toAbsPath(path);
	if (checkType(this->path) == FileController::NON)
		return (-1);
	return (0);
}

std::string FileController::getPath(void) {
	return (this->path);
}

FileController::FileMetaData* FileController::getMetaData(std::string path) {
	char								time_str[20];
	FileController::FileMetaData*		rtn;
	struct stat							buf;
	struct passwd*						u_data;
	struct tm*							time_info;

	if (stat(path.c_str(), &buf) == -1 ||
		!(	S_ISREG(buf.st_mode) ||
			S_ISDIR(buf.st_mode)))
		return (NULL);

	rtn = new FileController::FileMetaData();
	u_data = getpwuid(buf.st_uid);
	time_info = std::localtime(&(buf.st_mtimespec.tv_sec));
	std::strftime(time_str, 20, "%Y/%m/%d %H:%M:%S", time_info);

	size_t find = path.rfind("/");
	if (find == std::string::npos)
		rtn->name = path;
	else
		rtn->name = path.substr(find + 1);
	if (u_data != NULL)
		rtn->user = std::string(u_data->pw_name);
	else
		rtn->user = std::string("");
	rtn->type = modeToType(buf.st_mode);
	rtn->size = buf.st_size;
	rtn->gen_time = std::string(time_str);

	return (rtn);
}

void FileController::getFilesOfFolder(std::string& path, std::vector<FileMetaData*>& files_meta) {
	struct dirent* dirent;
	DIR* dir = opendir(path.c_str());
	while ((dirent = readdir(dir)) != NULL) {
		std::string sub_path;
		if (path.at(path.length() - 1) == '/')
			sub_path = path + std::string(dirent->d_name, dirent->d_namlen);
		else
			sub_path = path + "/" + std::string(dirent->d_name, dirent->d_namlen);
		files_meta.push_back(getMetaData(sub_path));
	}
	closedir(dir);
}

int FileController::getFileSize(std::string path) {
	int size = 0;
    std::ifstream ifs(path, std::ios::in | std::ios::binary);

	// @TODO : errorhandler non_crit으로 바꿔야하나?
    if (ifs.is_open() == 0) {
        std::cout << "NO FILE" << std::endl;
		return (-1);
	}
    ifs.seekg(0, std::ios::end);
    size = ifs.tellg();
    ifs.close();
	return (size);
}

int FileController::getFilesSize(void) const {
	if (this->mode == READ)
		return (this->files_meta.size());
	else
		return (-1);
}

FileController::FileMetaData* FileController::getFiles(int idx) const {
	if (idx < 0)	
		return (this->meta);
	else if (this->files_meta.size() <= size_t(idx))
		return (NULL);
	else
		return (this->files_meta[idx]);
}

FileController::Type FileController::modeToType(mode_t mode) {
	if (S_ISREG(mode))
		return (FILE);
	else if (S_ISDIR(mode))
		return (DIRECTORY);
	else
		return (NON);
}

FileController::Type FileController::checkType(std::string path) {
	struct stat stat_tmp;
	if (stat(path.c_str(), &stat_tmp) == -1)
		return (NON);
	return (modeToType(stat_tmp.st_mode));
}

std::string& FileController::toAbsPath(std::string& path) {
	if (path.empty() == false) {
		if (path.at(0) != '/') {
			char* pwd = getcwd(NULL, 0);
			path = std::string(pwd) + "/" + path;
			delete pwd;
		}
	}
	return (path);
}

std::string FileController::getPrePath(std::string path) {
	std::string tmp = path;
	if (path.substr(path.length() - 1, path.length()) == "/")
		tmp = tmp.substr(0, tmp.length() - 1);
	std::cout << tmp << " vs " << path << std::endl;
	int	
		idx = tmp.find_last_of("/");
    return (tmp.substr(0, idx + 1));
}
