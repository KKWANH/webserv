#ifndef					CONFIGSTATUS_HPP
# define				CONFIGSTATUS_HPP

# include				<map>
# include				<string>
# include				<iostream>
# include				<fstream>

# include				"ErrorHandler.hpp"

class 					StatusConfig
{
	private:
		std::string
			_uri;
	
	public:
		std::map<std::string, std::string>
			_status_map;
		void
			startConfig(
				std::string _uri_file);
		std::string
			getStatus(
				std::string _key);
};

# endif