#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include <map>
#include <string>
#include "SocketController.hpp"

class HTTPData {
	public:
		// Nginx Server Block num
		int										server_block;
		//server port, client port
		int 									server_port;
		std::string								client_ip;

        // Start Line
		std::string								method;
		std::string								uri_dir;
		std::string								uri_file;
		std::string								query_string;
		std::string								file_extension;
		std::string								CGI_root;
		std::string								CGI_what;
		int										status_code;
		bool        							isCGI;
		double									http_version;

		// Header Field
		std::map<std::string, std::string>		header_field;

		// Message Body
		std::string								message_body;

		HTTPData(int server_block, SocketController *Socket) {
			struct sockaddr_in serv_addr = Socket->getServerAddr();
			struct sockaddr_in cli_addr = Socket->getClientAddr();
			client_ip = std::string(inet_ntoa(cli_addr.sin_addr));
			server_port = (int)ntohs(serv_addr.sin_port);
			this->server_block = server_block;
			status_code = 0;
		}
};

#endif
