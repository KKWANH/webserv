#include <stdio.h>
#include <stdlib.h> // atoi를 사용하기 위해
#include <string.h> // memset을 사용하기 위해
#include <unistd.h> // sockarrd_in, read, write를 사용하기 위해
#include <arpa/inet.h> // htnol, htons, INADDR_ANY, sockaddr_in을 사용하기 위해
#include <sys/socket.h> // socket(), bind(),  listen(), accept()

void	error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

// port는 인자로 받는다
int		main(int ac, char *av[])
{
	int	serv_sock; // 서버 소켓
	int clnt_sock; // 클라이언트 소켓

	// sockaddr_in : 소켓 주소의 틀을 형성해주는 구조체로 AF_INET일 경우 사용
	// AF_INET : IPv4 인터넷 프로토콜
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr; // accept함수에서 사용됨
	socklen_t clnt_adr_size;

	// PF_INET : IPv4 인터넷 프로토콜 (AF_INET 과 동일하게 사용)
	// SOCK_STREAM : TCP 연결지향형 (어떤 타입의 소켓을 생성할 건지)
	// 마지막 0은 소켓에서 사용할 프로토콜인데 두번째 인자에서 미리 정해진 경우 0을 적는다.
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // TCP 연결지향이고 ipv4 도메인을 위한 소켓 생성
	if (serv_sock == -1)
		error_handling("socket() error");

	// 서버소켓 주소를 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET; // IPv4 인터넷 프로토콜
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // 컴퓨터에 존재하는 랜카드 중 사용 가능한 랜카드의 IP주소를 사용해라
	serv_adr.sin_port = htons(atoi(av[1])); // 포트번호 (프로그램 인자로 받음)

	// 소켓과 서버 주소를 바인딩
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");

	// 연결 대기열에 5개 생성
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	// 클라이언트로부터 요청이 오면 연결을 수락
	clnt_adr_size = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
	if (clnt_sock == -1)
		error_handling("accept() error");

	// 데이터 전송하기!
	char msg[] = "Hello I'm SKIM :)\n";
	write(clnt_sock, msg, sizeof(msg));

	// 소켓 닫기
	close(clnt_sock);
	close(serv_sock);

	return (0);
}

/*
 1) gcc server.c => ./a.out [port]
 2) brew install telnet
 3) 설정 > 네트워크에서 host ip 를 알아낼 수 있다.
 4) telnet [hostip] [port]
 */
