#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "./CGIData.hpp"

int		main(void)
{
    std::map<std::string, std::string> _envMap;
	char	**_env;
    CGIData cgiData;
    
    // local 환경변수에 이미 존재하는 아이들
    _envMap[std::string("USER")] = std::string(std::getenv("USER"));
    _envMap[std::string("PATH")] = std::string(std::getenv("PATH"));
    _envMap[std::string("LANG")] = std::string(std::getenv("LANG"));
    _envMap[std::string("PWD")] = std::string(std::getenv("PWD"));

    // parsing으로 가져온 아이들
    // _envMap[std::string("REQUEST_METHOD")] = requestMessage->getMethod(); // GET
    _envMap[std::string("REQUEST_METHOD")] = std::string("GET");
    // _envMap[std::string("PATH_INFO")] = requestMessage->getUriDir(); // /data/index.html
    _envMap[std::string("PATH_INFO")] = std::string("/index.html");
    _envMap[std::string("SERVER_PROTOCOL")] = std::string("HTTP/1.1");
    // _envMap[std::string("REQUEST_SCHEME")] = requestMessage->getMethod(); // GET
    _envMap[std::string("REQUEST_SCHEME")] = std::string("GET");
    _envMap[std::string("GATEWAY_INTERFACE")] = std::string("CGI/1.1");
    _envMap[std::string("SERVER_SOFTWARE")] = std::string("webserv/1.0");

    // _envMap[std::string("CONTENT_TYPE")] = requestMessage->_reqContentType;
    _envMap[std::string("CONTENT_TYPE")] = "application/x-www-form-urlencoded";
    // _envMap[std::string("CONTENT_LENGTH")] = requestMessage->_reqContentLength;
    _envMap[std::string("CONTENT_LENGTH")] = std::to_string(22);

    // _envMap[std::string("REMOTE_ADDR")] = requestMessage->_clientIP; // server socket addr
    // _envMap[std::string("REMOTE_PORT")] = requestMessage->_clientPort; // client port
    // _envMap[std::string("SERVER_ADDR")] = requestMessage->_hostIP; // client socket addr
    // _envMap[std::string("SERVER_PORT")] = requestMessage->_hostPort; // host port

    // _envMap[std::string("QUERY_STRING")] = requestMessage->_URIQueryString; // id=123
    _envMap[std::string("QUERY_STRING")] = "var=1234";

    // _envMap[std::string("SCRIPT_NAME")] = requestMessage->_URIFilePath; // /data/index.html
    _envMap[std::string("SCRIPT_NAME")] = "sample.php";

    // _envMap[std::string("REQUEST_URI")] = requestMessage->_originURI;
    _envMap[std::string("DOCUMENT_ROOT")] = "/Users/hyunja/all_42/00.Table_inworking/webserv/main/test/";
    _envMap[std::string("REQUEST_URI")] = "/Users/hyunja/all_42/00.Table_inworking/webserv/main/test/sample.php";
    _envMap[std::string("DOCUMENT_URI")] = "/Users/hyunja/all_42/00.Table_inworking/webserv/main/test/sample.php";
    // _envMap[std::string("SERVER_NAME")] = requestMessage->_serverName; // config 파일의 서버 이름
    _envMap[std::string("SCRIPT_FILENAME")] = "/Users/hyunja/all_42/00.Table_inworking/webserv/main/test/sample.php";

    // for (std::map<std::string, std::string>::iterator iter = requestMessage->_HTTPCGIENV.begin(); iter != requestMessage->_HTTPCGIENV.end(); iter++) {
    //     _envMap[iter->first] = iter->second;
    // } // CGI binary 파일의 헤더 (HTTP_)
    // request의 헤더가 cgi 의 환경변수로 들어가야 한다.
    
    _env = cgiData.generateEnvp(_envMap);
    cgiData.setCGIArgv();

    int fd[2];
    pipe(fd);
    std::cout << "cgiData.getFilePath() : " << cgiData.getFilePath() << std::endl;
    std::cout << "cgiData.getArgv()[0] : " << cgiData.getArgv()[0] << std::endl;
    std::cout << "cgiData.getArgv()[1] : " << cgiData.getArgv()[1] << std::endl;
    std::cout << "cgiData.getArgv()[2] : " << cgiData.getArgv()[2] << std::endl;
    if (execve(cgiData.getFilePath(), cgiData.getArgv(), _env) == -1)
        std::cout << "망해써요" << std::strerror(errno) << std::endl;
}