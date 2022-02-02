# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>
void err_handle(char * message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
} 

int main(int argc, char* argv[])
{
    int server_sock;
    int client_sock;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
        err_handle("socket_making_error_ xx....");
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
        err_handle("binding_error_ xx....");

    if (listen(server_sock, 10) == -1)
        err_handle("listen_error_ xx....");

    client_addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_addr_size);
    
    if (client_sock == -1)
        err_handle("do_not_accept_error_ xx....");

    char message[] = "\nplease finish this sub!\n";
    write(client_sock, message, sizeof(message));

    close(client_sock);
    close(server_sock);

    return 0;

}
