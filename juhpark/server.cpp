#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>

int main()
{
	//기존 소켓의 친구들
	int s_server, s_client;
	struct sockaddr_in addr_server, addr_client;
	char buffer[1024];
	socklen_t address_size;

	//kqueue에 쓰일 친구들
	int kfd;
	struct timespec pollingTime;
	struct kevent setEvent;
	struct kevent getEvent[10];
	int result = 0;
	int read_len = 0;

	//HTTP응답 헤더 스트링
	std::string httpTestHeaderString;


	//1.1에 200(잘된단뜻)
	httpTestHeaderString += "HTTP/1.1 200 OK\r\n";
//	httpTestHeaderString += "HTTP/1.1 403 Forbidden\r\n";
	httpTestHeaderString += "Content-Type: text/html; charset=utf-8\r\n";
	httpTestHeaderString += "\r\n";
	httpTestHeaderString += "<html>";
	httpTestHeaderString += "<head><title>이거 탭에 보임?</title></head>";
	httpTestHeaderString += "<body>";
	httpTestHeaderString += "<b><center> HI THERE! </center></b>";
	httpTestHeaderString += "</body>";
	httpTestHeaderString += "</html>";

	//kqueue만들고 시간생성
	kfd = kqueue();
	pollingTime.tv_sec = 1;
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
	addr_server.sin_port = htons(8080);

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

	//이벤트 셋팅과 관리
	EV_SET(&setEvent, s_server, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kfd, &setEvent, 1, NULL, 0, &pollingTime) == -1)
	{
		std::cout << "KEVENT SET ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}

	while (1)
	{
		result = kevent(kfd, NULL, 0, getEvent, 10, &pollingTime);
		if (result == -1)//에러
		{
			std::cout << "KEVENT SET ERROR : " << std::strerror(errno) << std::endl;
			return (1);
		}
		else if (result == 0)//이벤트 없음
		{
			std::cout << "통신 존버중 누가 좀 연결해보소" << std::endl;
		}
		else
		{
			std::cout << "이벤트 갯수 : " << result << std::endl;
			for (int i = 0; i < result; i++)
			{
				if (getEvent[i].ident == s_server)//연결성공시
				{
					std::cout << "fd : " << getEvent[i].ident << " index : " << i << std::endl;
					s_client = accept(s_server, (struct sockaddr*)&addr_client, &address_size);//요청을 받아들인다
					if (s_client == -1)
					{
						std::cout << "ACCEPT ERROR : " << std::strerror(errno) << std::endl;
						return (1);
					}
					std::cout << "YEAH ACCESS SUCESS" << std::endl;//라고 서버측에서 말하고
					write(s_client, "YEAH", 4);//라고 클라에게 보낸다
					EV_SET(&setEvent, s_client, EVFILT_READ, EV_ADD, 0, 0, NULL);//그리고 클라쪽에서 응답을 받는듯?
					if (kevent(kfd, &setEvent, 1, NULL, 0, &pollingTime) == -1)
					{
						std::cout << "KEVENT SET ERROR : " << std::strerror(errno) << std::endl;
						return (1);
					}
				}
				else//표준입력이라던가, 표준 출력이라던가 암튼 그런거일때..?
				//얜 요청을 보낼떄인가봐
				//저기선 read랑 write이벤트를 시퀸스가 바뀔때마다 각각 처리해야 한다고 한다
				{
					std::cout << "클라 fd : " << s_client << std::endl;
					read_len = read(getEvent[i].ident, buffer, 1024);
					if (read_len == -1)
					{	
						std::cout << "READ ERROR : " << std::strerror(errno) << std::endl;
						return (1);
					}
					std::cout << "fd : " << getEvent[i].ident << " index : " << i << std::endl;
					std::cout << "response : " << std::string(buffer, read_len) << std::endl;
					write(getEvent[i].ident, httpTestHeaderString.data(), httpTestHeaderString.length());
					//이건 몰랐네, c_str랑 c_str의 strlen써야 하는 줄
					EV_SET(&setEvent, getEvent[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	//				EV_SET(&setEvent, getEvent[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	//				//이놈 주석을 풀면 뭔가 나
					if (kevent(kfd, &setEvent, 1, NULL, 0, NULL) == -1)
					{
						std::cout << "KEVENT SET ERROR : " << std::strerror(errno) << std::endl;
						return (1);
					}
					close(getEvent[i].ident); // 만약 HTTP 1.1이라면 영속성으로 인해 필요할 때만 연결을 종료해야함
					//근데 저걸 주석 처리하면 bind자체가 안됨
					//심지어 저걸 주석 풀어도 안되고
					//브라우저를 직접 열고 닫아야 되더라
				}
			}
		}
	}

	/*
	//accept로 클라의 소켓을 받아온다
	address_size = sizeof(addr_client);
	s_client = accept(s_server, (struct sockaddr*)&addr_client, &address_size);
	if (s_client == -1)
	{
		std::cout << "ACCEPT ERROR : " << std::strerror(errno) << std::endl;
		return (1);
	}
	write(s_client, "YEAH", 4);

	//받아온 소켓을 가지고 감탄사를 소켓에게 보냄
	//이 아래는 에코통신이긴한데, HTTP통신에선 저걸 써먹기가 애매할 듯
	//쓴다해도 중간에 지우고 붙이고 하면 정신나갈듯 ㅎㅎㅎㅎㅎ
	int len = 0;
	while ((len = recv(s_client, buffer, 1024, 0)) != 0)
	{
		send(s_client, buffer, len, 0);
	}
	*/
//	close(s_client);
	close(s_server);

	return (0);
}
