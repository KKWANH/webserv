#ifndef MIMECONTROLLER_HPP
# define MIMECONTROLLER_HPP

#include <map>
#include <string>
#include <iostream>
#include <fstream>

class MIMEController {
	private:
		std::string							uri_MIME;
		std::map<std::string, std::string>	MIME;

	public:
		MIMEController(void)
		{
			uri_MIME = "./MIME.types";

		}

		std::string		getMIME(std::string key)
		{
			if (this->MIME.find(key) != MIME.end())
				return (this->MIME[key]);
			return (this->MIME["bin"]);
		}

		int				setMIME(void)
		{
			std::string		key, value, temp;
			int				start, end;
			std::ifstream	fileRead(uri_MIME.c_str());

			if (fileRead.peek() == std::ifstream::traits_type::eof()) {
				std::cout << "MIME file is empty" << std::endl;
				return (-1);
			}

			while (getline(fileRead, temp)) {
				start = 0;
				end = temp.find(": ");
				key = temp.substr(start, end - start);

				start = end + 2;
				end = temp.find("\n");
				value = temp.substr(start, end - start);

				MIME.insert(std::make_pair(key, value));
			}
			return (0);
		}

		void			printMIME(void)
		{
			std::map<std::string, std::string>::iterator	it;

			for (it = MIME.begin(); it != MIME.end(); it++)
				std::cout << it->first << "\t: " << it->second << std::endl;
		}
};

#endif
