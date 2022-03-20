#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include <map>
#include <string>
#include <iostream>

class HTTPData {
	public:
		// Nginx Server Block num
		int										server_block;
		// server port & client IP
		int										server_port;
		std::string								client_ip;
        // Start Line
		std::string								method;
		std::string								uri_dir;
		std::string								uri_file;
		std::string								query_string;
		std::string								file_extension;
		std::string								CGI_root;
		std::string								CGI_what;
		bool        							isCGI;
		double									http_version;

		// Header Field
		std::map<std::string, std::string>		header_field;

		// Message Body
		std::string								message_body;

		int										status_code;

		int	getSBlock(void);
		
		HTTPData(int server_block, int server_port, std::string client_ip);
};

#endif
