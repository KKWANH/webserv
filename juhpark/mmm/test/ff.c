#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
int main()
{

	int fd = open("sample.php", O_CREAT | O_WRONLY, 0755);

	dup2(fd, 1); // 모든 출력이 fd라는 파일 디스크립터로 write된다.

	char buffer[20] = "welcome!\n";

	printf("1.\n");
	printf("%s", buffer); // fd에 써진다.
	fflush(stdout);		  //write 시스템콜이 printf() 보다 빠르기 때문에, 버퍼를 비워줌

	printf("2.\n");
	write(1, buffer, strlen(buffer));  // fd에 써진다
	dup2(STDIN_FILENO, fd);
	printf("3.\n");
	write(fd, buffer, strlen(buffer)); // fd에 써진다

	close(fd);
	return 0;
}

