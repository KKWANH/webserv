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

class
	FileController
{
    public:
		typedef enum eType
		{
			FILE,
			DIRECTORY,
			NON
		} Type;

		typedef enum eMode
		{
			READ,
			WRITE
		} Mode;

		typedef struct sFileMetaData
		{
			std::string
				_name,
				_user,
				_genr_time;
			long
				_size;
			FileController::Type
				_type;
		} FileMetaData;
	
	private:
		int
			_fd;
		Type
			_type;
		Mode
			_mode;
		std::string
			_path;
		FileMetaData*
			_meta;
		std::vector<FileMetaData*>
			_files_meta;
		
		static FileMetaData*
			getMetaData(
				std::string _tmp_path);
		static inline void
			getFilesOfFolder(
				std::string&				_tmp_path,
				std::vector<FileMetaData*>& _tmp_files_meta);

	public:
		FileController(
				void);
		FileController(
				std::string _tmp_path, Mode _tmp_mode);
		~FileController(
				void);

		int
			setPath(
				std::string _tmp_path);
		std::string
			getPath(
				void);
		int
			getFilesSize(
				void) const;
		FileMetaData*
			getFiles(
				int _idx) const;

		static int
			getFileSize(
				std::string _pth);
		static inline Type
			modeToType(
				mode_t _mod);
		static Type
			checkType(
				std::string _pth);
		static std::string&
			toAbsPath(
				std::string& _pth);
		static std::string
			getPrePath(
				std::string path);
};

#endif