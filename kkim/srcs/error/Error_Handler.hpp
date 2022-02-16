#ifndef					ERRORHANDLER_HPP
# define				ERRORHANDLER_HPP

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
	Error_Handler : public std::exception
{
	private:
		std::string
			_msg;
		int
			_tmp;

	public:
		Error_Handler(void)
		{}

		Error_Handler(std::string _str)
		:	_msg(""),
			_tmp(-1) { }

		Error_Handler(std::string _str, int _num)
		:	_msg(_str),
			_tmp(_num) { };

		virtual ~Error_Handler()
			throw() { }
		virtual const char *
			what() const throw()
		{
			static std::string
				_rst;

			_rst.clear();
			_rst += ANSI_RED;
			_rst += "[ERR] ";
			_rst += ANSI_RES;
			_rst += _msg;

			//yamkim님의 것에선 저걸 넣었는데 read,write만 아니면 되는건가
			if (errno)
			{
				_rst += "\n";
				_rst += ANSI_BLU;
				_rst += "[INF] ";
				_rst += ANSI_RES;
				_rst += (std::strerror(errno));
			}

			//요 if 문은 그냥 명목상으로 만든거
			if (_tmp == 0)
				_rst += "!";
			return (_rst.c_str());
		}
};

#endif