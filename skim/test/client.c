#include <stdio.h>
#include <stdlib.h> // atoi를 사용하기 위해
#include <string.h> // memset을 사용하기 위해
#include <unistd.h> // sockarrd_in, read, write를 사용하기 위해
#include <arpa/inet.h> // htnol, htons, INADDR_ANY, sockaddr_in을 사용하기 위해
#include <sys/socket.h> // socket(), bind(), connect(), close()

void	error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

// 인자로 ip주소, port번호를 받는다.
int		main(int ac, char *av[])
{
	int	clnt_sock;
	struct sockaddr_in	serv_adr;
	char message[1024] = {0};

	// TCP연결지향이고 ipv4 도메인을 위한 소켓을 생성
	clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (clnt_sock == -1)
		error_handling("socket() error");

	// 인자로 받은 서버 주소 정보를 저장
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET; // 서버주소체계는 ipv4이다
	serv_adr.sin_addr.s_addr = inet_addr(av[1]); // 서버주소 IP를 저장해준다. (인자로 받음)
	serv_adr.sin_port = htons(atoi(av[2])); // 서버주소 port번호를 저장해준다. (인자로 받음)

	// 클라이언트 소켓부분에 서버를 연결
	if (connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");

	// 연결이 성공적으로 되었으면 데이터 받기
	if (read(clnt_sock, message, sizeof(message)) == -1)
		error_handling("read() error");
	printf("Message from server: %s\n", message);

	// 통신 후 소켓 닫기
	close(clnt_sock);
	return (0);
}

/*
 ./server [port]
 ./client [ip] [port] (같은 컴퓨터 라면 127.0.0.1로도 가능하다)
*/
