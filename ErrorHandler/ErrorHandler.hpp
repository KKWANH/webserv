// NOTE: modified coding convention by joopark
#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

class ErrorHandler : public std::exception {
	public:
		typedef enum ErrorLevel {
			DEFAULT,
			CRIT,
			NON_CRIT
		} e_Lev;
	private:
		char const*	file;
		char const*	func;
		int 		line;
		std::string msg;
		e_Lev		level;

	public:
		ErrorHandler();
		ErrorHandler(char const* file, char const* func, int line, std::string msg);
		ErrorHandler(char const* file, char const* func, int line, std::string msg, e_Lev level);
		virtual ~ErrorHandler() throw();
		const char* what() const throw();
		e_Lev	getLevel() const;

};

#endif
