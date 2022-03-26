#ifndef		FILECONTROLLER_HPP
# define	FILECONTROLLER_HPP

# include	<cstring>
# include	<fstream>
# include	<iostream>
# include	<sys/stat.h>
# include	<pwd.h>
# include	<unistd.h>
# include	<vector>
# include	<dirent.h>
# include	<fcntl.h>
# include	"ErrorHandler.hpp"

class FileController {
public:
	typedef enum Type {
		FILE,
		DIRECTORY,
		NON
	} Type;

	typedef enum Mode {
		READ,
		WRITE
	} Mode;

	typedef struct FileMetaData {
		std::string name;
		std::string user;
		std::string gen_time;
		long size;
		FileController::Type type;
	} FileMetaData;

private:
	int							fd;
	Type						type;
	Mode						mode;
	std::string					path;
	FileMetaData*				meta;
	std::vector<FileMetaData*>	files_meta;
	
	static FileMetaData* 		getMetaData(std::string path);
	static inline void 			getFilesOfFolder(std::string& path, std::vector<FileMetaData*>& files_meta);

public:
	FileController(void);
	FileController(std::string path, Mode mode);
	~FileController(void);

	int					setPath(std::string path);
	std::string			getPath(void);
	int					getFilesSize(void) const;
	FileMetaData*		getFiles(int idx) const;
	static int			getFileSize(std::string path);
	static inline Type	modeToType(mode_t mode);
	static Type			checkType(std::string path);
	static std::string&	toAbsPath(std::string& path);
	static std::string	getPrePath(std::string path);
};

#endif