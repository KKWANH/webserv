#include "ErrorHandler.hpp"

ErrorHandler::ErrorHandler(void) {}

ErrorHandler::ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(DEFAULT) { }

ErrorHandler::ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str, ErrorHandler::e_Lev _level)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(_level) { }

virtual ErrorHandler::~ErrorHandler() throw() { }

e_Lev	ErrorHandler::getLevel() const { return(_lev); }
		
std::string	ErrorHandler::getMsg() const { return(_msg); }

virtual const char *ErrorHandler::what() const throw()
{
	static std::string _rst;

	_rst.clear();
	_rst += "\n\033[38;5;196m";
	_rst += "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n";
	_rst += "[ERR] ";
	if (_lev == DEFAULT)
		_rst += "DEFAULT OR NOT YET : ";
	if (_lev == CRIT)
		_rst += "CRITICAL : ";
	if (_lev == NON_CRIT)
		_rst += "NON_CRITICAL : ";
	_rst += "\033[38;5;220m";
	_rst += _msg;
	_rst += "\033[38;5;196m";
	_rst += "\n[FILE] ";
	_rst += "\033[38;5;220m";
	_rst += _fil;
	_rst += "\033[38;5;196m";
	_rst += "\n[FUNC] ";
	_rst += "\033[38;5;220m";
	_rst += _fnc;
	_rst += "\033[38;5;196m";
	_rst += "\n[LINE] ";
	_rst += "\033[38;5;220m";
	_rst += std::to_string(_lin);

	//yamkim님의 것에선 저걸 넣었는데 read,write만 아니면 되는건가
	if (errno)
	{
		_rst += "\n";
		//_rst += ANSI_BLU;
		_rst += "[INF] ";
		//_rst += ANSI_RES;
		_rst += (std::strerror(errno));
	}
	_rst += "\033[38;5;196m";
	_rst += "\n~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~";
	_rst += "\033[0m";
	return (_rst.c_str());
}