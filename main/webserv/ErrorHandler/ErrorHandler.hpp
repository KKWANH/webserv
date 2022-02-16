/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juhpark <juhpark@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/15 17:51:09 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <exception>
# include <string>
# include <sys/errno.h>

class ErrorHandler : public std::exception
{
private:
	std::string msg; //어디서 오류가 났나 등등 오류 메세지
	int tmp; //이건 혹시 모를까 넣은 변수
public:
	ErrorHandler();
	ErrorHandler(std::string wha);
	ErrorHandler(std::string wha, int tmp);
	virtual ~ErrorHandler() throw();
	virtual const char *what() const throw();
};

#endif
