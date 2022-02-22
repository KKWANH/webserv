/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyunja <hyunja@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/19 16:15:25 by hyunja           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*   By: joopark <joopark@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 12:19:51 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/19 16:12:51 by joopark          ###   ########.fr       */
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
	
	std::string e_file;
	std::string e_func;

public:
	ErrorHandler();
	ErrorHandler(std::string wha);
	//ErrorHandler(std::string w_file, std::string w_func, std::string wha);
	ErrorHandler(std::string wha, int tmp);
	virtual ~ErrorHandler() throw();
	virtual const char *what() const throw();
};

#endif
