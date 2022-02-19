#ifndef MIMECONTROLLER_HPP
# define MIMECONTROLLER_HPP

#include <map>
#include <string>
#include <iostream>
#include <fstream>


// TODO: 멤버 메소들들이 ConfigController와 중복되어 합쳐도 될 것 같음.
class MIMEController {
	private:
		std::string uri_MIME;
		std::map<std::string, std::string> MIME;
	
	public:
		MIMEController() {
			uri_MIME = "./setting/mime.types";
		}

		// key에 해당하는 config value 값 반환
		// 해당하는 key가 없으면 bin에 해당하는 value 반환
		std::string		getMIME(std::string key) {
			if (this->MIME.find(key) != MIME.end())
				return (this->MIME.find(key)->second);
			return (this->MIME.find("bin")->second);
		}

		// MIME map에 키-값 쌍 형식으로 설정 값 저장
		// mime.types이 비어있는 경우 에러 처리
		int			setMIME() {
			std::string key, value, temp;
			int start, end;
			std::ifstream fileRead(uri_MIME.c_str());

			// mime.types이 비어있음.
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
		
		// Test를 위한 MIME 설정 출력
		void					printMIME() {
			std::map<std::string, std::string>::iterator iter;
			iter = MIME.begin();
			while (true) {
				std::cout << iter->first << "\t: " << iter->second << std::endl;
				iter++;
				if (iter == MIME.end())
					break;
			}
		}
};

#endif