/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyunja <hyunja@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/03/16 21:19:25 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

//throw ErrorHandler(__FILE__, __func__, __LINE__, "Error Message", ErrorHandler::에러의 치명도);
//요로케 던지면 됩니다
//리펙토링으로 Error_Handler가 되려나..?

//크게 2가지로
//그냥 소켓만 끊는거랑
//프로세스를 뽀개버리는 걸루

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
		ErrorHandler(void) {}

		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(DEFAULT) { }

		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str, ErrorHandler::e_Lev _level)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(_level) { }

		virtual ~ErrorHandler() throw() { }

		e_Lev	getLevel() const { return(_lev); }
		
		std::string	getMsg() const { return(_msg); }

		virtual const char *what() const throw()
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
};

#endif
