/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juhpark <juhpark@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/21 17:34:38 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

/*
class ErrorHandler : public std::exception
{
private:
	const char *file;
	const char *func;
	int 		line;
	std::string msg; //어디서 오류가 났나 등등 오류 메세지
	int 		tmp; //이건 혹시 모를까 넣은 변수
public:
	ErrorHandler();
	ErrorHandler(const char* fi, const char* fc, int lin, std::string wha);
	ErrorHandler(const char* fi, const char* fc, int lin, std::string wha, int tmp);
	virtual ~ErrorHandler() throw();
	virtual const char *what() const throw();
};
*/
class Error_Handler : public std::exception
{
	private:
		const char*	_fil;
		const char*	_fnc;
		int 		_lin;
		std::string _msg;
		int			_tmp;

	public:
		Error_Handler(void) {}

		Error_Handler(const char* _ch1, const char* _ch2, int _int, std::string _str)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _tmp(-1) { }

		Error_Handler(const char* _ch1, const char* _ch2, int _int, std::string _str, int _num)
			: _fil(_ch1), _fnc(_ch2), _lin(_int), _msg(_str), _tmp(_num) { }

		virtual ~Error_Handler() throw() { }
		virtual const char *what() const throw()
		{
			static std::string _rst;

			_rst.clear();
			//_rst += ANSI_RED;
			_rst += "\033[38;5;196m";
			_rst += "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n";
			_rst += "[ERR] ";
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
			if (_tmp == 0)
				_rst += "!";
			_rst += "\033[38;5;196m";
			_rst += "\n~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~";
			_rst += "\033[0m";
			return (_rst.c_str());
		}
};


#endif
