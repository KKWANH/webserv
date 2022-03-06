#ifndef HTTPDATA_HPP
# define HTTPDATA_HPP

#include <map>
#include <string>
class HTTPData {
	public:
        // Request Message - Start Line
		std::string								Method;
		std::string								uri_dir;
		std::string								uri_file;
		std::string								query_string;
		bool        							isCGI;


		// Request Message - Header Field, Body
		std::map<std::string, std::string>		header_field;
		std::string								message_body;

		char*									binary_body; // ??? 이건 머지이...

};

#endif