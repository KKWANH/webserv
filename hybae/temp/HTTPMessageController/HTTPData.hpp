#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include <map>
#include <string>

class HTTPData {
	public:
		// Nginx Server Block num
		int										server_block;
        // Start Line
		std::string								method;
		std::string								uri_dir;
		std::string								uri_file;
		std::string								query_string;
		std::string								file_extension;
		double									http_version;
		bool        							isCGI;

		// Header Field
		std::map<std::string, std::string>		header_field;

		// Message Body
		std::string								message_body;

		HTTPData(int server_block) {
			this->server_block = server_block;
		}
};

#endif