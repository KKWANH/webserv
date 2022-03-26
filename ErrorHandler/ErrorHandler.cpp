#include "ErrorHandler.hpp"

ErrorHandler::ErrorHandler() {};
ErrorHandler::ErrorHandler(char const* file, char const* func, int line, std::string msg)
			: file(file), func(func), line(line), msg(msg), level(DEFAULT) { }
ErrorHandler::ErrorHandler(char const* file, char const* func, int line, std::string msg, e_Lev level)
			: file(file), func(func), line(line), msg(msg), level(level) { }
ErrorHandler::~ErrorHandler() throw() {};

const char* ErrorHandler::what() const throw() {
			static std::string rtn;

			rtn += "\n\033[38;5;196m";
			rtn += "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n";
			rtn += "[ERR] ";
			if (level == DEFAULT)
				rtn += "DEFAULT OR NOT YET : ";
			if (level == CRIT)
				rtn += "CRITICAL : ";
			if (level == NON_CRIT)
				rtn += "NON_CRITICAL : ";
			rtn += "\033[38;5;220m";
			rtn += msg;
			rtn += "\033[38;5;196m";
			rtn += "\n[FILE] ";
			rtn += "\033[38;5;220m";
			rtn += file;
			rtn += "\033[38;5;196m";
			rtn += "\n[FUNC] ";
			rtn += "\033[38;5;220m";
			rtn += func;
			rtn += "\033[38;5;196m";
			rtn += "\n[LINE] ";
			rtn += "\033[38;5;220m";
			rtn += std::to_string(line);

			if (errno)
			{
				rtn += "\n";
				//rtn += ANSI_BLU;
				rtn += "[INF] ";
				//rtn += ANSI_RES;
				rtn += (std::strerror(errno));
			}
			rtn += "\033[38;5;196m";
			rtn += "\n~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~";
			rtn += "\033[0m";
			return (rtn.c_str());
}

ErrorHandler::e_Lev	ErrorHandler::getLevel() const {
	return level;
}
