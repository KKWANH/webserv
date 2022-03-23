#include "ErrorHandler.hpp"

ErrorHandler::ErrorHandler() : msg(""), tmp(-1), e_file(""), e_func("") { }

ErrorHandler::ErrorHandler(std::string wha) : msg(wha), tmp(-1), e_file(""), e_func("") { }

ErrorHandler::ErrorHandler(std::string wha, int asd) : msg(wha), tmp(asd), e_file(""), e_func("") { }

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
	reason += "ERROR! : ";
	reason += msg;
	/*
	if (e_file && e_func)
	{
		reason += "file : "
		reason += e_file;
		reason += "\nfunc : "
		reason += e_func;
	}
	*/
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
