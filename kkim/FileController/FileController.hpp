#ifndef		FILECONTROLLER_HPP
# define	FILECONTROLLER_HPP

# include	<cstring>
# include	<fstream>
# include	<iostream>
# include	<sys/stat.h>
# include	<pwd.h>
# include	<unistd.h>

class
	FileController
{
    private:

    public:
		typedef enum eType
		{
			FIL,
			DIR,
			NON
		} Type;

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
};

#endif