#ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

#include <string>
#include <map>
#include <sstream>
#include <sys/socket.h>

// #include "ErrorHandler.hpp"
#include "HTTPData.hpp"
// #include "../Utils.hpp"
#include "../configHandler/NginxConfig.hpp"

class HTTPHandler {
	protected:
		static const ssize_t				TRANS_ERROR = -1;
		std::amp<map::string, std::string>	_headers;
		int									_connectionFd;
		std::string							_headerString;
		NginxConfig::ServerBlock			_serverConf;
		NginxConfig::GlobalConfig			_nginxConf;
		class Buffer {
			private:
				int		_bufSize;
				char	*_buf;
			public:
				Buffer(size_t bufSize)
				{
					_buf = new char[bufSize];
					if (_buf == NULL)
						throw std::string("Buffer::Buffer(): new failed");
				}

				~Buffer()
				{
					delete[] _buf;
				}

				char	*operator*(void)
				{
					return (_buf);
				}
		};

	public:
		HTTPHandler(int connectionFd, NginxConfig::ServerBlock serverConf, const NginxConfig::GlobalConfig &nginxConfig) \
		: _serverConf(serverConf), _nginxConf(nginxConfig)
		{
			_connectionFd = connectionFd;
			_headerString = std::string("");
		}

		void	setGeneralHeader(std::string status)
		{
			static char	timeBuf[48];
			time_t		rawTime;

			std::time(&rawTime);
			struct tm	*timeInfo = std::localtime(&rawTime);
			std::strftime(timeBuffer, 48, "%a, %d %b %Y %H:%M:%S %Z", timeinfo); // ex) Mon, 07 Feb 2022 08:40:05 GMT

			_headerString = status;
			_headerString += "\r\n";

			_headers["Date"] = timeBuf;
			_headers["Server"] = std::string("webserv/1.0");
			_headers["Connection"] = std::string("close");
		}

		void	convertHeaderMapToString(void)
		{
			std::map<std::string, std::string>::iterator	it;
			for (it = _headers.begin(); it != _headers.end(); ++it) {
				size_t	duplicateIdentfier = (it->first).find("@");
				if (duplicateIdentfier != std::string::npos) { // header 에서 @의 의미를 찾아보자
					_headerString += (it->first).substr(duplicateIdentfier + 1);
				} else {
					_headerString += (it->first);
				}
				_headerString += ": ";
				_headerString += it->second;
				_headerString += "\r\n";
			}
			_headerString += "\r\n";
		}

		std::pair<std::string, str::string>	getHTTPHeader(const std::string &str, std::size_t &endPos)
		{
			std::pair<std::string, std::string>	pair;
			pair.first = Parser::getIdentifier(str, endPos, ": ", false); // http header 파싱 부분 만들기
			if (pair.first.empty())
				throw std::string("HTTPHandler::getHTTPHeader(): getIdentifier() failed");
			endPos += 2;
			pair.second = Parser::getIdentifier(str, endPos, "\r\n", false);
			if (pair.second.empty())
				throw std::string("HTTPHandler::getHTTPHeader(): getIdentifier() failed");
			endPos += 2;
			return (pair);
		}

		// Alert 창 띄우기
		void	requestAlert(std::string ip, std::string port, std::string serverPort, std::string path, std::string method)
		{
			static char	timeStringBuf[20];
			time_t 		rawTime;
			struct tm	*timeInfo;

			std::time(&rawTime);
			timeInfo = std::localtime(&rawTime);
			std::strftime(timeStringBuffer, 20, "%Y/%m/%d %H:%M:%S", timeinfo);
			std::cout << "\x0d\033[0;32m[";
			std::cout << timeStringBuffer;
			std::cout << "]\033[0m ";
			std::cout << "\033[0;33m[" << method << " @ " << serverPort << "]\033[0m ";
			std::cout << ip << ":" << port << path << std::endl;
		}
};

#endif
