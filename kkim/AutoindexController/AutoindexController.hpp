#ifndef AUTOINDEXCONTROLLER_HPP
# define AUTOINDEXCONTROLLER_HPP

#include <dirent.h>
#include <cstring>
#include <iostream>

class AutoindexController {
	public:
        
    /*
        해당 디렉토리가 존재하지 않는 경우 -1 반환
        해당 디렉토리 내 index 존재할 경우 0 반환
        해당 디렉토리 내 index 존재하지 않는 경우, autoindex로 표시할 파일 및 폴더 수 반환
    */
	static int			isCorrectDir(std::string root_path, std::string dir_path, std::string index_file) {
		DIR*			dir;
		struct dirent	*diread;
		std::string		absolutePath = root_path + dir_path;
		int				count = -1;

		if ((dir = opendir(absolutePath.c_str())) == NULL)
			return (count);    // 404 Not Found + extension html
		while ((diread = readdir(dir)) != NULL) {
			if (diread->d_ino == 0)
				continue;
			if (strcmp(index_file.c_str(), diread->d_name) == 0) {
				closedir(dir);
				return (0);		// 200 OK with index file
			}
			count++;
		}
		closedir(dir);
		return (count);
	}

	static std::string	getAutoIndexBody(std::string root_path, std::string dir_path) {
		DIR*			dir;
		struct dirent	*diread;
		std::string		absolutePath = root_path + dir_path;
		std::string		message_body = "",
						return_str = "";

		if ((dir = opendir(absolutePath.c_str())) == NULL)
			return (""); 

		message_body += "<html>\n";
		message_body += "\t<head>\n";
		message_body += "\t\t<title>\n";
		message_body += ("\t\t\tIndex of " + dir_path + "\n");
		message_body += "\t\t</title>\n";
		message_body += "\t</head>\n";
		message_body += "<link rel=\"icon\" type=\"image/x-icon\" href=\"assets/favicon.ico\" />\n";
		message_body += "\t<body>\n";
		message_body += "\t\t<h1>\n";
		message_body += ("\t\t\tIndex of " + dir_path + "\n");
		message_body += "\t\t</h1>\n";
		message_body += "\t\t<hr>\n";
		message_body += "\t\t<pre>\n";
		while ((diread = readdir(dir)) != NULL) {
			if (diread->d_ino == 0 ||
				strcmp(diread->d_name, ".") == 0 ||
				strcmp(diread->d_name, ".DS_Store") == 0)
				continue;
			std::string name = std::string(diread->d_name);
			if (isDirOrFile(absolutePath + "/" + name))
				message_body += ("\t\t\t<a href=\"" + name + "/\" is dir>" + name + "</a>\n");
			else
				message_body += ("\t\t\t<a href=\"" + name + "\" is file>" + name + "</a>\n");
		}
		message_body += "\t\t</pre>\n";
		message_body += "\t\t<hr>\n";
		message_body += "\t</body>\n";
		message_body += "</html>";
		closedir(dir);
		return (message_body);
	}

	static int			isDirOrFile(std::string path) {
		DIR*			dir;

		if ((dir = opendir(path.c_str())) == NULL)
			return (0);
		closedir(dir);
		return (1);
	}
};

#endif