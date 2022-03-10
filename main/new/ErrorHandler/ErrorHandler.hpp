/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skim <skim@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/03/02 13:09:59 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

# define DEFAULT 0
# define CRIT 1
# define NON_CRIT 2

//throw ErrorHandler(__FILE__, __func__, __LINE__, "Error Message");
//요로케 던지면 됩니다
//리펙토링으로 Error_Handler가 되려나..?

//크게 2가지로
//그냥 소켓만 끊는거랑
//프로세스를 뽀개버리는 걸루

class ErrorHandler : public std::exception
{
	private:
		const char*	_fil;
		const char*	_fnc;
		int 		_lin;
		std::string _msg;
		int			_lev;

	public:
		ErrorHandler(void) {}

		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(-1) { }

		ErrorHandler(const char* _ch1, const char* _ch2, int _int, std::string _str, int _num)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _lev(_num) { }

		virtual ~ErrorHandler() throw() { }

		int	getLevel() { return(this->_lev); }
		
		std::string	getMsg() { return(this->_msg); }

		virtual const char *what() const throw()
		{
			static std::string _rst;

			_rst.clear();
			//_rst += ANSI_RED;
			_rst += "\033[38;5;196m";
			_rst += "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n";
			_rst += "[ERR] ";
			if (_lev == DEFAULT)
				_rst += " - DEFAULT : ";
			if (_lev == CRIT)
				_rst += " - CRITICAL : ";
			if (_lev == NON_CRIT)
				_rst += " - NON_CRITICAL : ";
			//_rst += ANSI_RES;
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

			//요 if 문은 그냥 명목상으로 만든거
//			if (_tmp == 0)
//				_rst += "!";
			_rst += "\033[38;5;196m";
			_rst += "\n~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~";
			_rst += "\033[0m";
			return (_rst.c_str());
		}
};

#endif