#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char* argv[])
{
    int client_sock;
    struct sockaddr_in server_addr;
    char message[1024] = {0x00, };

    

}