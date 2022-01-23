#ifndef CONFIGCONTROLLER_HPP
# define CONFIGCONTROLLER_HPP

#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>

class ConfigController {
	private:
		std::map<std::string, std::string> config;
		std::string configUri;

	public:
		// key에 해당하는 config value 값 반환
		std::string		getConfig(std::string key) {
			return (this->config.at(key));
		}

		// TODO: config 설정이 비어있거나 config 파일이 없을 경우, default 설정 넣어야 하는지 확인 필요
		// config map에 키-값 쌍 형식으로 설정 값 저장
		// 파일이 없거나, 비어있는 경우 에러 처리
		int						setConfig() {
			configUri = "/Users/server/Desktop/BHS/webserv_temp/webserv.config";
			std::string		key, value, temp;
			int						start, end;
			std::ifstream fileRead(configUri.c_str());

			// config 파일 없음
			if (fileRead.is_open() == false) {
				std::cout << "There is no config file" << std::endl;
				return (-1);
			}

			// config 파일 비어있음.
			if (fileRead.peek() == std::ifstream::traits_type::eof()) {
				std::cout << "Config file is empty" << std::endl;
				return (-1);
			}

			// config 파일 읽어서 키-값 쌍으로 저장
			// key: value 형식의 문자열을 (key, value) 형식으로 map에 삽입
			while (getline(fileRead, temp)) {
				std::cout << temp << std::endl;
				start = 0;
				end = temp.find(": ");
				key = temp.substr(start, end - start);
				
				start = end + 2;
				end = temp.find("\n");
				value = temp.substr(start, end - start);

				config.insert(std::make_pair(key, value));
			}
			return (0);
		}

		// Test를 위한 config 설정 출력
		void					printConfig() {

			// config 설정 시, 파일이 비어있는 경우 처리 됨.
			/*
			if (config.empty()) {
				std::cout << "CONFIG IS EMPTY" << std::cout;
				return ;
			}
			*/
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