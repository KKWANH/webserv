#include <unistd.h>
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstdio>

typedef std::map<std::string, std::string> MAP_TYPE;

void mac_test(const char* func, int line, const char* file)
{
	std::cout << "test \n" << "func : " << func << "\nlne : " << line << "\nfile : " << file << std::endl; 
}


char** set_env(MAP_TYPE envs)
{
	envs["CONTENT_LENGTH"] = "0";
	envs["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
	envs["GATEWAY_INTERFACE"] = "CGI/1.1";
	envs["PATH_INFO"] = "/Users/mac/Documents/websv/webserv/juhpark/mmm/test";
	envs["QUERY_STRING"] = "val=1234";
	envs["REQUEST_METHOD"] = "GET";
	envs["SCRIPT_NAME"] = "sample.php";
	envs["SERVER_SOFTWARE"] = "WEBSERV/0.1";
	envs["SERVER_PROTOCOL"] = "HTTP/1.1";
	envs["DOCUMENT_ROOT"] = "/Users/mac/Documents/websv/webserv/juhpark/mmm/test";
	envs["DOCUMENT_URI"] = "/Users/mac/Documents/websv/webserv/juhpark/mmm/test/sample.php";
	envs["REQUEST_URI"] = "/Users/mac/Documents/websv/webserv/juhpark/mmm/test/sample.php";

	envs["SCRIPT_FILENAME"] = "/Users/mac/Documents/websv/webserv/juhpark/mmm/test/sample.php";
	envs["REDIRECT_STATUS"] = "200";

	std::string tmp;

	char** rtn = new char*[envs.size() + 1];
	int i = 0;
	for (MAP_TYPE::iterator it = envs.begin(); it != envs.end(); it++) //양방향 반복자니깐 <>안됨
	{
		tmp = it->first;
		tmp += "=";
		tmp += it->second;
		char* input = new char[tmp.size() + 1];
		std::strcpy(input, tmp.c_str());
		rtn[i] = input;
		i++;
	}
	rtn[i] = NULL;
	mac_test(__func__, __LINE__, __FILE__);
	return (rtn);
}

//extern char **environ;

int main()
{
	//입력과 환경변수
	MAP_TYPE envs;
	char *str[4];
	char **input_envs;
	//파이프들

	//경로 설정
	std::string path = "./php-cgi";
	std::string command = "./sample.php";
	std::string val = "value=1234";
	str[0] = const_cast<char *>(path.c_str());
	str[1] = const_cast<char *>(command.c_str());
	str[2] = const_cast<char *>(val.c_str());
	str[3] = NULL;
	//메타 데이터 설정
	input_envs = set_env(envs);
//	if (execve(str[0], str, input_envs) == -1)
//		std::cout << "error :" << std::strerror(errno)  << std::endl;

	
	//echo설정
	/*
	char *tmp[3];
	std::string e1 = "echo";
	std::string e2 = "aa";
	tmp[0] = const_cast<char *>(e1.c_str());
	tmp[1] = const_cast<char *>(e2.c_str());
	tmp[2] = NULL;
*/	
	int fds1[2];
	int fds2[2];
	int M_input, C_input;
	int M_output, C_output;
	pipe(fds1);
	pipe(fds2);
	if (pipe(fds1) == -1 || pipe(fds2) == -1)
	{
		std::cout << "pipe err\n";
		return (1);
	}

	M_input = fds1[1];
	M_output = fds2[0];

	C_input = fds2[1];
	C_output = fds1[0];
	int sta;
	pid_t pid = fork();

	if (pid < 0)
	{
		std::cout << "pid맛감 " << std::endl;
		return (1);
	}
	else if (pid == 0)//자식
	{
		if (dup2(C_input, STDIN_FILENO) == -1|| dup2(C_output, STDOUT_FILENO) == -1)
			std::cout << "error :" << std::strerror(errno)  << std::endl;
		std::cout << "child" << std::endl;
		close(fds1[1]);
		close(fds2[0]);
		//얘넨 쓸모없으니
		if (execve(str[0], str, input_envs) == -1)
			std::cout << "error :" << std::strerror(errno)  << std::endl;
	}
	else if (pid > 0)
	{
		close(fds1[0]);
		close(fds2[1]);
		std::cout << "parent" << std::endl;
		waitpid(pid, &sta, 0);
	}

	return (0);
}

