#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>
#include <vector>
#include <map>
#include <fstream>
#include "config.hpp"


void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, 0, 0, NULL);
    change_list.push_back(temp_event);
}

std::string read_html(Config con)
{
	std::string res; 
	std::ifstream fin(con.get_root() + "/" + con.get_page());
	if (!fin)
	{
		std::cout << "fopen ERROR : " << std::strerror(errno) << std::endl;
		return (NULL);
	}
	std::string tmp;
	while (1)
	{
		getline(fin, tmp);
		if (fin.eof())
			break ;
		res += tmp;
	}
	return (res);
}

int main(int argc, char **argv)
{
	//기존 소켓의 친구들
	int s_server;
	struct sockaddr_in addr_server, addr_client;
	char buffer[1024];
	socklen_t address_size;
	std::map<int, std::string> client_list;

	//kqueue에 쓰일 친구들
	int kfd;
	struct timespec pollingTime;
	std::vector<struct kevent> setEvent;
	struct kevent getEvent[10];
	int result = 0;
	int read_len = 0;

	//config설정
	Config con;
	int code = con.parse_config(argc, argv);
	//응답 헤더에 쓰일 친구들
	std::string httpTestHeaderString;
	std::string f_read;

	f_read = read_html(con);
	if (f_read.length() == 0)
		return (1);
	//HTTP응답 헤더 스트링


	//1.1에 200(잘된단뜻)
	if (code == 200)
	{
		//httpTestHeaderString += "HTTP/1.1 200 OK\r\n";
		httpTestHeaderString += "HTTP/1.1 ";
		httpTestHeaderString += std::to_string(code);
		httpTestHeaderString += " OK\r\n";
		httpTestHeaderString += "Content-Type: text/html; charset=utf-8\r\n";
		httpTestHeaderString += "Content-Length: ";
		httpTestHeaderString += std::to_string(f_read.size());
		httpTestHeaderString += "\r\n\r\n";
		httpTestHeaderString += f_read;

	}
	else if (code == 403)
	{
		httpTestHeaderString += "HTTP/1.1 ";
		httpTestHeaderString += std::to_string(code);
		httpTestHeaderString += " Forbidden\r\n";
		httpTestHeaderString += "Content-Type: text/html; charset=utf-8\r\n";
		httpTestHeaderString += "Content-Length: ";
		httpTestHeaderString += std::to_string(f_read.size());
		httpTestHeaderString += "\r\n\r\n";
		httpTestHeaderString += f_read;
	}	
	
	//kqueue만들고 시간생성
	kfd = kqueue();
	pollingTime.tv_sec = con.get_timeout();
	pollingTime.tv_nsec = 0;

	//PF_INET는 IPv4를 쓰겠다는 거고
	//SOCK_STREAM는 UDP쓰겠다고
	//포트 번호는 하나만 쓸 거니 0
	s_server = socket(PF_INET, SOCK_STREAM, 0);
	if (s_server == -1)
	{
		std::cout << "SOCKET ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	//주소 정보를 초기화 및 넣기
	//INET IPv4그거 맞음
	//INADDR_ANY는 IP주소를 초기화 할때
	//소켓이 동작하는 컴퓨터의 IP주소를 자동으로 할당해줌
	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(8082);

	if (bind(s_server, (struct sockaddr*)&addr_server, sizeof(addr_server)) == -1)
	{
		std::cout << "BIND ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	//이떄, 서버 소켓은 문지기 역할을 하게 된다던데
	if (listen(s_server, 4) == -1)
	{
		std::cout << "LISTEN ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}
	fcntl(s_server, F_SETFL, O_NONBLOCK);

	//이벤트 셋팅과 관리
	change_events(setEvent, s_server, EVFILT_READ, EV_ADD);

	//읽고 쓰는거 합치기
	//kevent함수는 한번만 쓰기
	while (1)
	{
		result = kevent(kfd, &setEvent[0], setEvent.size(), getEvent, 10, &pollingTime); 
//		std::cout << "이벤트 갯수 : " << result << std::endl;
		if (result == -1)//에러
		{
			std::cout << "KEVENT SET ERROR : " << std::strerror(errno) << std::endl;
			return (1);
		}
		else if (result == 0)//이벤트 없음
			std::cout << "통신 존버중 누가 좀 연결해보소" << std::endl;

		setEvent.clear();

		//이벤트가 발생했을 떄
		//일단 clear를 집어넣어보자

		for (int i = 0; i < result; i++)
		{
			if (getEvent[i].ident == s_server)//연결성공시
			{
				int s_client;
				s_client = accept(s_server, (struct sockaddr*)&addr_client, &address_size);//요청을 받아들인다
				if (s_client == -1)
				{
					std::cout << "ACCEPT ERROR : " << std::strerror(errno) << std::endl;
					return (1);
				}
				fcntl(s_client, F_SETFL, O_NONBLOCK);
				std::cout << s_client << " : YEAH ACCESS SUCESS" << std::endl;//라고 서버측에서 말하고
				change_events(setEvent, s_client, EVFILT_READ, EV_ADD | EV_ENABLE);
				change_events(setEvent, s_client, EVFILT_WRITE, EV_ADD | EV_ENABLE);
				client_list[s_client] = "";//여기에 클라의 메세지를 적으면 된다
			}
			else if (getEvent[i].filter == EVFILT_READ)//표준입력이라던가, 표준 출력이라던가 암튼 그런거일때..?
			//얜 요청을 보낼떄인가봐
			//저기선 read랑 write이벤트를 시퀸스가 바뀔때마다 각각 처리해야 한다고 한다
			{
				read_len = read(getEvent[i].ident, buffer, 1024);
				if (read_len == -1)
				{	
					std::cout << "READ ERROR : " << std::strerror(errno) << std::endl;
					close(getEvent[i].ident);
					client_list.erase(getEvent[i].ident);
					close(s_server);
					return (1);
				}
				else if (read_len == 0)
				{
					std::cout << "client " << getEvent[i].ident << " is DISCONNECTED bye~" << std::endl;
					close(getEvent[i].ident);
					client_list.erase(getEvent[i].ident);
					//요건 입력이 나올게 없다면 끊어준다 이런것임
					//근데 처음엔 이거 없이 쓰다가 낭패를 보았다 
				}
				else if (client_list.find(getEvent[i].ident) != client_list.end()) //저게 뭐냐면 아직 클라에게 이벤트가 남아있을때란뜻
				{
					change_events(setEvent, getEvent[i].ident, EVFILT_READ, EV_ADD | EV_DISABLE);
					change_events(setEvent, getEvent[i].ident, EVFILT_WRITE, EV_ADD | EV_ENABLE);
					buffer[read_len] = '\0';
					client_list[getEvent[i].ident] += buffer;
					std::cout << "client " << getEvent[i].ident << " says : " << client_list[getEvent[i].ident] << std::endl;
				}

			}
			//write이벤트를 시행해야 할때
			//근데 서버에서 일방적으로 전송을 뭐 어찌 하란거지
			//그리고 무엇을 전송을 하란거지?
			//HTTP사이트 같은걸 뿌리냐
			else if (getEvent[i].filter == EVFILT_WRITE)
			{
				if (client_list.find(getEvent[i].ident) != client_list.end() && read_len)
				{
					std::cout << "buffer "<< getEvent[i].ident <<  " : " << buffer << std::endl;
					if (write(getEvent[i].ident, httpTestHeaderString.data(), httpTestHeaderString.length()) == -1)
					{
						std::cout << "WRITE ERROR : " << std::strerror(errno) << std::endl;
						close(getEvent[i].ident);
						client_list.erase(getEvent[i].ident);
						return (1);
					}
					change_events(setEvent, getEvent[i].ident, EVFILT_WRITE, EV_ADD | EV_DISABLE);
					change_events(setEvent, getEvent[i].ident, EVFILT_READ, EV_ADD | EV_ENABLE);
					client_list[getEvent[i].ident].clear();
				}
			}
			
		}
	}

	//이 아래는 에코통신이긴한데, HTTP통신에선 저걸 써먹기가 애매할 듯
	//쓴다해도 중간에 지우고 붙이고 하면 정신나갈듯 ㅎㅎㅎㅎㅎ
	/*
	int len = 0;
	while ((len = recv(s_client, buffer, 1024, 0)) != 0)
	{
		send(s_client, buffer, len, 0);
	}
	*/
	close(s_server);

	return (0);
}
