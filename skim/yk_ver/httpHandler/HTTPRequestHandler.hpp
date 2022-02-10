#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP

#include <cstring>
#include <unistd.h>
#include <ctime>
#include <iostream>

#include "HTTPHandler.hpp"

class HTTPRequestHandler : public HTTPHandler {
	private:
		std::string		_stringBuf;
		bool			_stringBufClear;
		Phase			_phase;
		long			_contentLen;
		long			_contentLenSum;
		bool			_chunkFinish;
		long			_dynamicBufSize;
		FileContoller	*_fileController;

		bool		getStartLine(HTTPData &data)
		{
			std::string	*bufPtr = getDateByCRLF(1000);

			if (bufPtr == NULL)
				return (false);

			std::vector<std::string> tmp = Parser::getSplitBySpace(*bufPtr);
			if (tmp.size() != 3)
				throw std::string("invalie http header");

			    if (tmp[0] == std::string("GET")
				|| tmp[0] == std::string("HEAD")
				|| tmp[0] == std::string("POST")
				|| tmp[0] == std::string("PUT")
				|| tmp[0] == std::string("DELETE")
				|| tmp[2] != std::string("HTTP/1.1")) {
					data._reqMethod = tmp[0];
					date._reqURI = tmp[1];
					data_originURI = tmp[1];
					data.setURIelemmnts();
				} else {
					throw std::string("invalie http header");
				}
			return (true);
		}

		// CRLF (파일 쪼개서 보내기) : http 응답분할
		std::string	*getDateByCRLF(int searchLen)
		{
		}

	public:
		typedef enum e_Phase {
			PARSE_STARTLINE,
			PARSE_HEADER,
			BODY_TYPE_CHECK,
			PARSE_BODY_NBYTES,
			PARSE_BODY_CHUNK,
			REMOVE_CRLF,
			FINISH
		} Phase;

		HTTPRequestHandler(int connectionFd, const NginxConfig::ServerBlock &serverConf, Nginx::GlobalConfig &nginxConf) \
		: HTTPHandler(connectionFd, serverConf, nginxConf)
		{
			phase = PARSE_STARTLINE;
			_contentLen = -1;
			_contentLenSum = 0;
			_chunkFinish = false;
			_dynamicBufSize = 0;
			_fileController = NULL;
			_stringBuf = std::string("");
			_stringBufClear = false;
		}

		// 여러 정보들을 모아서 요청 메세지 형태로 만드는 과정
		virtual	HTTPRequestHandler::Phase process(HTTPData &data, long bufferSize)
		{

		}

		virtual ~HTTPRequestHandler(void)
		{
			closeFileFd();
		}

		int		getFiledFd(void)
		{
			return (_fileController->getFd());
		}

		void	closeFiledFd(void)
		{
			if (_fileController != NULL) {
				delete _fileController;
				_fileController = NULL;
			}
		}
};

#endif
