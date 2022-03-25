#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

//throw ErrorHandler(__FILE__, __func__, __LINE__, "Error Message", ErrorHandler::에러의 치명도);
//요로케 던지면 됩니다
//리펙토링으로 Error_Handler가 되려나..?

class ErrorHandler : public std::exception
{
	public:
		typedef enum ErrorLevel {
			DEFAULT,
			CRIT,
			NON_CRIT
		} e_Lev;
	private:
		const char*	_fil;
		const char*	_fnc;
		int 		_lin;
		std::string _msg;
		e_Lev		_lev;

	public:
		ErrorHandler(void);
		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str);
		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str, ErrorHandler::e_Lev _level);

		virtual ~ErrorHandler() throw();
		e_Lev	getLevel() const;
		std::string	getMsg() const;
		virtual const char *what() const throw();
};

#endif