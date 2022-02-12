#ifndef CONFIGCONTROLLER_HPP
# define CONFIGCONTROLLER_HPP

#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>

#define DEFAULT_URI "/Users/soobin/Desktop/webserv/skim/hb_clone/webserv.conf"

class configController {
	private:
		std::map<std::string, std::string>	config;
		std::string							configUri;
	public:
		configController() : configUri(DEFAULT_URI) {}

		std::string		getConfig(std::string const &key) {
			return (this->config[key]);
		}

		// config 파일을 읽어 각 key, value를 map 형식으로 저장하여 준다.
		int				setConfig(void) {
			configUri = "./webserv.config";
			std::string		key, value, temp;
			std::ifstream	fileRead(configUri.c_str());

			if (fileRead.peek() == std::ifstream::traits_type::eof()) {
				std::cout << "Error: Config file is empty" << std::endl;
				return (-1);
			}

			while (getline(fileRead, temp)) {
				// std::cout << temp << std::endl;
				int findEqual = temp.find("=");
				key = temp.substr(0, findEqual - 0);
				value = temp.substr(findEqual + 1, temp.size());
				config.insert(std::make_pair(key, value));
			}
			return (0);
		}

		// 저장되어있는 config를 출력해 준다.
		void			printConfig(void) {
			std::map<std::string, std::string>::iterator	it;

			for (it = config.begin(); it != config.end(); it++) {
				std::cout << it->first << " = " << it->second << std::endl;
			}
		}

};

#endif
