#include "HTTPConnection.hpp"

HTTPConnection::HTTPConnection(int fd, int block, SocketController* socket)
{
    seq = HTTP_READ;
    socket_fd = fd;


    int server_port = (int)ntohs((socketController->getServerAddr()).sin_port);
    std::string	client_ip = std::string(inet_ntoa((socketController->getServerAddr()).sin_addr));
	std::string	host_ip = std::string(inet_ntoa((socketController->getClientAddr()).sin_addr));
	std::stringstream	clientPortString;
	std::stringstream	hostPortString;
	clientPortString << ntohs(socketController->getServerAddr().sin_port);
	hostPortString << ntohs(socketController->getClientAddr().sin_port);


    data = new HTTPData(block, server_port, client_ip);
    request = new RequestMessage(data);
    response = new ResponseMessage(data);

}

int HTTPConnection::run(void) {

}