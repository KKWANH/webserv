#ifndef					ERROR_HANDLER_HPP
# define				ERROR_HANDLER_HPP

# include				<string>
# include				<exception>
# include				<string>
# include				<sys/errno.h>
# include				"../utils/Utils.hpp"

/**
-------------------------------------------------------------
> ErrorHandler : std_exception

- private variables
_msg					오류 메시지
_tmp					혹시나 해서 ㅎㅎ

- constructor
empty, string, string/int

- destructor
virtual, throw

- public functions
what					return what is the message
-------------------------------------------------------------
*/
class
	ErrorHandler : public std::exception
{
	private:
		const char*
			_fil;
		const char*
			_fnc;
		int
			_lin;
		std::string
			_msg;
		int
			_tmp;

	public:
		ErrorHandler(void)
		{}

		ErrorHandler(const char* _ch1, const char* _ch2, int _num, std::string _str)
		:	_fil(_ch1),
			_fnc(_ch2),
			_lin(_num),
			_msg(_str),
			_tmp(-1)
		{ }

		ErrorHandler(const char* _ch1, const char* _ch2, int _nm1, std::string _str, int _nm2)
		:	_fil(_ch1),
			_fnc(_ch2),
			_lin(_nm1),
			_msg(_str),
			_tmp(_nm2)
		{ }

		virtual
			~ErrorHandler() throw()
		{ }

		virtual const char*
			what(void) const throw()
		{
			static std::string
				_rst;

			_rst.clear();
			_rst += ANSI_RED;
			_rst += "~--------------------------------------------------------------------\n";
			_rst +=   "[ERROR] : ";
			_rst += ANSI_YEL;
			_rst += _msg;
			_rst += ANSI_RED;
			_rst += "\n[FILE]  : ";
			_rst += ANSI_YEL;
			_rst += _fil;
			_rst += ANSI_RED;
			_rst += "\n[FUNC]  : ";
			_rst += ANSI_YEL;
			_rst += _fnc;
			_rst += ANSI_RED;
			_rst += "\n[LINE]  : ";
			_rst += ANSI_YEL;
			_rst += std::to_string(_lin);

			if (errno)
			{
				_rst += "\n";
				_rst += ANSI_BLU;
				_rst += "[INFO]  : ";
				_rst += ANSI_RES;
				_rst += strerror(errno);
			}

			if (_tmp == 0)
				;

			_rst += ANSI_RED;
			_rst += "\n~--------------------------------------------------------------------\n";
			_rst += ANSI_RES;
			return (_rst.c_str());
		}
};

#endif