/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juhpark <juhpark@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:34:20 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/15 19:46:00 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"

//에러로 받을만한게 에러 메세지랑 errno랑 뭐가 더 있을려나

ErrorHandler::ErrorHandler() : msg(""), tmp(-1) { }

ErrorHandler::ErrorHandler(std::string wha) : msg(wha), tmp(-1) { }

ErrorHandler::ErrorHandler(std::string wha, int asd) : msg(wha), tmp(asd) { }

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
	reason += msg;
	if (errno) //yamkim님의 것에선 저걸 넣었는데 read,write만 아니면 되는건가
	{
		reason += "\n";
		reason += "Reason : ";
		reason += "\033[38;5;220m";
		reason += (std::strerror(errno));
	}
	//요 if 문은 그냥 명목상으로 만든거
	if (tmp == 0)
		reason += "!";
	reason += "\033[0m";
	return (reason.c_str());
}
