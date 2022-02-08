#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>

class Config
{
private:
	std::string root;
	int timeout;
	std::string page;
public:
	Config()
	{
		root = "";
		timeout = 0;
		page = "";
	}

	void parse_n_input(std::string rt)
	{	
		std::ifstream fin(rt); //여기선 디폴트읽었는데 실패한 경우는 생략할래
		std::string tmp;
		getline(fin, tmp);
		root = tmp.substr(5, tmp.length() - 5);
		getline(fin, tmp);
		timeout = std::stoi(tmp.substr(8, tmp.length() - 8));
		getline(fin, tmp);
		page = tmp.substr(5, tmp.length() - 5);
		fin.close();
	}

	int parse_config(int argc, char **argv)
	{
		std::cout << "config check.. ";
		if (argc == 1) //없으면
		{
			std::cout << "DEFAULT" << std::endl;
			parse_n_input("./default.config");

			return (200);
		}
		else if (argc != 2)
		{
			std::cout << "ERROR" << std::endl;
			std::cout << "config는 1개 이하로" << std::endl;
			return (-1);
		}
		else
		{	
			std::string rt(argv[1]);
			std::ifstream fin(rt);
			if (!fin || !(strnstr(&argv[1][strlen(argv[1] - 7)], ".config", 7)))
			{
				std::cout << "config open ERROR" << std::endl;
				root = "/Users/mac/Documents/websv/webserv/juhpark";
				timeout = 1;
				page = "403.html";
				fin.close();
				return (403);
			}
			fin.close();
			std::cout << "CUSTOM" << std::endl;
			parse_n_input(rt);
			return (200);
		}
	}

	std::string get_root(void) { return(this->root); }
	
	int get_timeout(void) { return(this->timeout); }

	std::string get_page(void) { return(this->page); }
};


#endif
