/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunghyunpark <sunghyunpark@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:34:20 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/21 16:44:35 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"
#include <iostream>

//에러로 받을만한게 에러 메세지랑 errno랑 뭐가 더 있을려나

ErrorHandler::ErrorHandler() : msg(""), tmp(-1) { }

ErrorHandler::ErrorHandler(const char* fi, const char* fc, int lin, std::string wha) : file(fi), func(fc), line(lin), msg(wha), tmp(-1) { }

ErrorHandler::ErrorHandler(const char* fi, const char* fc, int lin, std::string wha, int asd) : file(fi), func(fc), line(lin), msg(wha), tmp(asd) { }

ErrorHandler::~ErrorHandler() throw() { }

//yamkim님의 것을 슬쩍 보았는데
//에러처리에선
//발생한 시간이랑 메세지 출력만 나와있었다

//에러 메세지를 만든다
const char* ErrorHandler::what() const throw() 
{
	static std::string reason;


	reason.clear();
	reason += "\033[38;5;196m";
	reason += "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n";
	reason += "ERROR! : ";
	reason += "\033[38;5;220m";
	reason += msg;
	reason += "\033[38;5;196m";
	reason += "\nFILE : ";
	reason += "\033[38;5;220m";
	reason += file;
	reason += "\033[38;5;196m";
	reason += "\nFUNC : ";
	reason += "\033[38;5;220m";
	reason += func;
	reason += "\033[38;5;196m";
	reason += "\nLINE : ";
	reason += "\033[38;5;220m";
	reason += std::to_string(line);
	if (errno) //yamkim님의 것에선 저걸 넣었는데 read,write만 아니면 되는건가
	{
		reason += "\n";
		reason += "\033[38;5;196m";
		reason += "Reason : ";
		reason += "\033[38;5;220m";
		reason += (std::strerror(errno));
	}
	//요 if 문은 그냥 명목상으로 만든거
	if (tmp == 0)
		reason += "!";
	reason += "\033[38;5;196m";
	reason += "\n~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~";
	reason += "\033[0m";
	return (reason.c_str());
}
