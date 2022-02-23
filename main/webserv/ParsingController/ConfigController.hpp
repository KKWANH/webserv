#ifndef CONFIGCONTROLLER_HPP
# define CONFIGCONTROLLER_HPP

#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include "../ErrorHandler/ErrorHandler.hpp"

#define DEFAULT_URI "./setting/default.config"

class ConfigController {
	private:
		std::map<std::string, std::string> config;
		std::string config_uri;

	public:
		ConfigController() : config_uri(DEFAULT_URI) {}

		// key에 해당하는 config value 값 반환
		std::string		getConfig(std::string key) { return (this->config.at(key)); }

		// argument로 들어온 경로 config_uri로 설정
		// 경로가 .config 파일이 아닐 경우, Default config 설정
		void					setUri(int argc, char **argv) {
			if (argc == 2) {
				if (strcmp(&argv[1][strlen(argv[1]) - 7], ".config") == 0)
					config_uri = std::string(argv[1]);
				else
					std::cout << "This is not CONFIG file. default config file will be applied" << std::endl;
			}
			else if (argc > 2)
				throw ErrorHandler(__FILE__, __func__, __LINE__, "too many arguments");
			return;
		}

		// config map에 키-값 쌍 형식으로 설정 값 저장
		// config file이 비어있는 경우 에러 처리
		void					setConfig(int argc, char **argv) {
			setUri(argc, argv);
			std::string		key, value, temp;
			int				start, end;
			std::ifstream	fileRead(config_uri.c_str());

			// config 파일 비어있음.
			if (fileRead.peek() == std::ifstream::traits_type::eof()) {
				throw ErrorHandler(__FILE__, __func__, __LINE__, "Config file is empty");
			}

			while (getline(fileRead, temp)) {
				start = 0;
				end = temp.find(": ");
				key = temp.substr(start, end - start);

				start = end + 2;
				end = temp.find("\n");
				value = temp.substr(start, end - start);

				config.insert(std::make_pair(key, value));
			}
			return ;
		}

		// Test를 위한 config 설정 출력
		void					printConfig() {
			std::map<std::string, std::string>::iterator iter;
			iter = config.begin();
			while (true) {
				std::cout << iter->first << "\t: " << iter->second << std::endl;
				iter++;
				if (iter == config.end())
					break;
			}
		}
};

#endif
