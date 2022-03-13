#ifndef					CONFIGMIME_HPP
# define				CONFIGMIME_HPP

# include				<map>
# include				<string>
# include				<iostream>
# include				<fstream>

# include				"ErrorHandler.hpp"

class 					MimeConfig
{
	private:
		std::string
			_uri;
		std::map<std::string, std::string>
			_mime_map;
	
	public:
		MimeConfig(
				std::string _fil_name);
		std::string
			getMIME(
				std::string _key);
		void
			printMIME(
				void);
};

# endif