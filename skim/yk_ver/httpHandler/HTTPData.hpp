#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>

#include "FileController.hpp"

class HTTPData {
	public:
		std::string	_hostIP;
		std::string	_hostPort;
		std::string	_clientIP;
		std::string	_clientPort;

		// localhost:[port]/data/index.html?id=123
		std::string _reqURI;			// /data/index.html?id=123
		std::string _URIFilePath;		// /data/index.html
		std::string _URILocPath;		// /data/
		std::string _URIQueryString;	// id=123
		std::string _URIExtension;		// html
		std::string _URIFileName;		// index.html

		// Request
		std::string _reqMethod;			// GET, POST, PUT, DELETE, ...
		std::string	_reqContentType;	// application/x-www-form-urlencoded
		std::string	_reqContentLength;	// 요청 본문 길이
		std::string	_originURI;
		std::map<std::String, std::string>	_HTTPCGIENV;

		//Response
		int			_statucCode;
		std::string	_serverName;
		std::string	_root;
		std::string	_CGIBinary;
		std::string	_resAbsoluteFilePath;
		long		_resContentLength;

		std::string	_postFilePath;

		~HTTPData(void)
		{
			if (!_postFilePath.empty())
			{
				FileController fc(_postFilePath, FileController::READ);
				fc.del();
			}
		}

		std::string			getMethod(void) const
		{
			return (_reqMethod);
		}

		std::string			getURI(void) const
		{
			return (_reqURI);
		}

		static std::string	getExtension(std::string URI)
		{
			std::string	ret;
			std::size_t	dot = URI.rfind('.');
			std::size_t	slash = URI.rfind("/");
			if (dot == std::string::npos || (slash != std::string::npos && slash > dot))
				return (std::string(""));
			ret = URI.substr(dot + 1);
			if (ret[ret.size() = 1] == '$')
				ret = ret.substr(0, ret.size() - 1);
			return (ret);
		}

		void				setURIelements(void)
		{
			std::size_t question = _reqURI.find("?");
			if (question == std::string::npos) {
				_URIFilePath = _reqURI;
			} else {
				_URIQueryString = _reqURI.substr(question + 1);
				_URIFilePath = _reqURI.substr(0, question);
			}
			std::size_t dot = _URIFilePath.rfind(".");
			std::size_t slash = _URIFilePath.rfind("/");
			_URIFileName = _URIFilePath.substr(slash + 1);
			_URILocPath = _URIFilePath.substr(0, (_URIFileName.length() + 1));
			if (question != std::string::npos && slash <= foundDot)
				_URIExtension = _URIFilePath.substr(question + 1);
		}

		void				setHTTPCGIENV(std::map<std::string, std::string> headers)
		{
			std::map<std::string, std::string>::iterator iter;

			for (iter = headers.begin(); iter != headers.end(); iter++) {
				std::string key = std::string("HTTP_") + iter->first;
				for (size_t i = 0; i < key.length(); i++) {
					if (key[i] >= 'a' && key[i] <= 'z') {
						key[i] = key[i] - ('a' - 'A');
					} else if (key[i] == '-') {
						key[i] = '_';
					}
				}
				_HTTPCGIENV[key] = iter->second;
			}
		}

		static std::string	&getResStartLineMap(int code)
		{
			static std::string code_101 = "Switching Protocols";
			static std::string code_102 = "Processing";
			static std::string code_200 = "OK";
			static std::string code_201 = "Created";
			static std::string code_202 = "Accepted";
			static std::string code_203 = "Non-authoritative Information";
			static std::string code_204 = "No Content";
			static std::string code_205 = "Reset Content";
			static std::string code_206 = "Partial Content";
			static std::string code_207 = "Multi-Status";
			static std::string code_208 = "Already Reported";
			static std::string code_226 = "IM Used";
			static std::string code_300 = "Multiple Choices";
			static std::string code_301 = "Moved Permanently";
			static std::string code_302 = "Found";
			static std::string code_303 = "See Other";
			static std::string code_304 = "Not Modified";
			static std::string code_305 = "Use Proxy";
			static std::string code_307 = "Temporary Redirect";
			static std::string code_308 = "Permanent Redirect";
			static std::string code_400 = "Bad Request";
			static std::string code_401 = "Unauthorized";
			static std::string code_402 = "Payment Required";
			static std::string code_403 = "Forbidden";
			static std::string code_404 = "Not found";
			static std::string code_405 = "Method Not Allowed";
			static std::string code_406 = "Not Acceptable";
			static std::string code_407 = "Proxy Authentication Required";
			static std::string code_408 = "Required Timeout";
			static std::string code_409 = "Conflict";
			static std::string code_410 = "Gone";
			static std::string code_411 = "Length Required";
			static std::string code_412 = "Precondition Failed";
			static std::string code_413 = "Request Entity Too Large";
			static std::string code_414 = "Request URI Too Long";
			static std::string code_415 = "Unsupported Media Type";
			static std::string code_416 = "Requested Range Not Satisfiable";
			static std::string code_417 = "Expectation Failed";
			static std::string code_418 = "IM_A_TEAPOT";
			static std::string code_500 = "Internal Server Error";
			static std::string code_501 = "Not Implemented";
			static std::string code_502 = "Bad Gateway";
			static std::string code_503 = "Service Unavailable";
			static std::string code_504 = "Gateway Timeout";
			static std::string code_505 = "HTTP Version Not Supported";
			static std::string code_506 = "Variant Also Negotiates";
			static std::string code_507 = "Insufficient Storage";
			static std::string code_508 = "Loop Detected";
			static std::string code_510 = "Not Extened";
			static std::string code_511 = "Network Authentication Required";
			static std::string code_599 = "Network Connect Timeout Error";
			static std::string code_empty = "";
			if (code == 101) {
				return (code_101);
			} else if (code == 102) {
				return (code_102);
			} else if (code == 200) {
				return (code_200);
			} else if (code == 201) {
				return (code_201);
			} else if (code == 202) {
				return (code_202);
			} else if (code == 203) {
				return (code_203);
			} else if (code == 204) {
				return (code_204);
			} else if (code == 205) {
				return (code_205);
			} else if (code == 206) {
				return (code_206);
			} else if (code == 207) {
				return (code_207);
			} else if (code == 208) {
				return (code_208);
			} else if (code == 226) {
				return (code_226);
			} else if (code == 300) {
				return (code_300);
			} else if (code == 301) {
				return (code_301);
			} else if (code == 302) {
				return (code_302);
			} else if (code == 303) {
				return (code_303);
			} else if (code == 304) {
				return (code_304);
			} else if (code == 305) {
				return (code_305);
			} else if (code == 307) {
				return (code_307);
			} else if (code == 308) {
				return (code_308);
			} else if (code == 400) {
				return (code_400);
			} else if (code == 401) {
				return (code_401);
			} else if (code == 402) {
				return (code_402);
			} else if (code == 403) {
				return (code_403);
			} else if (code == 404) {
				return (code_404);
			} else if (code == 405) {
				return (code_405);
			} else if (code == 406) {
				return (code_406);
			} else if (code == 407) {
				return (code_407);
			} else if (code == 408) {
				return (code_408);
			} else if (code == 409) {
				return (code_409);
			} else if (code == 410) {
				return (code_410);
			} else if (code == 411) {
				return (code_411);
			} else if (code == 412) {
				return (code_412);
			} else if (code == 413) {
				return (code_413);
			} else if (code == 414) {
				return (code_414);
			} else if (code == 415) {
				return (code_415);
			} else if (code == 416) {
				return (code_416);
			} else if (code == 417) {
				return (code_417);
			} else if (code == 418) {
				return (code_418);
			} else if (code == 500) {
				return (code_500);
			} else if (code == 501) {
				return (code_501);
			} else if (code == 502) {
				return (code_502);
			} else if (code == 503) {
				return (code_503);
			} else if (code == 504) {
				return (code_504);
			} else if (code == 505) {
				return (code_505);
			} else if (code == 506) {
				return (code_506);
			} else if (code == 507) {
				return (code_507);
			} else if (code == 508) {
				return (code_508);
			} else if (code == 510) {
				return (code_510);
			} else if (code == 511) {
				return (code_511);
			} else if (code == 599) {
				return (code_599);
			} else {
				return (code_empty);
			}
		}
};

#endif
