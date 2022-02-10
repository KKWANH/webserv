#ifndef TEST_UTIL_HPP
# define TEST_UTIL_HPP

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#include <sys/select.h>
#include <poll.h>
#include <sys/event.h>

#define PORT 9000

using namespace std;

int			initSocket(int* s_socket, struct sockaddr_in* s_addr) {

	*s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(s_addr, 0, sizeof(*s_addr));
	s_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr->sin_family = AF_INET;
	s_addr->sin_port = htons(PORT);

	if(bind(*s_socket, (struct sockaddr*)s_addr, sizeof(*s_addr)) == -1) {
		cout << "Could not Bind" << endl;
		return (-1);
	}

	if(listen(*s_socket, 5) == -1) {
		cout << "listen Fail" << endl;
		return (-1);
	}

	return (0);
}

int			initSelect(fd_set* read, int* fd_max, int s_socket, struct timeval* timeout) {
	// clear
	FD_ZERO(read);
	// add fd(s_socket) to read
	FD_SET(s_socket, read);
	// num of fd to find
	*fd_max = s_socket;
	
	timeout->tv_sec = 5;
	timeout->tv_usec = 5000;

	// set non-blocking
	if (fcntl(s_socket, F_SETFL, O_NONBLOCK) == -1)
		return (-1);
	return (0);
}

int			initPoll(struct pollfd* poll_fd, int s_socket) {
	
	// settings for the server
	poll_fd[0].fd = s_socket;
	poll_fd[0].events = POLLIN;
	poll_fd[0].revents = 0;

	// Initialize pollfd
	for (int i = 1; i < BUFSIZ; i++)
		poll_fd[i].fd = -1;

	// set non-blocking
	if (fcntl(s_socket, F_SETFL, O_NONBLOCK) == -1)
		return (-1);
	return (0);
}

int			initKqueue(int* kq, int* change_count, int s_socket, struct kevent* change_list) {
	
	// init kqueue
	*kq = kqueue();
	
	// settings for the server
	EV_SET(&change_list[0], s_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	*change_count = 1;	

	// set non-blocking
	if (fcntl(s_socket, F_SETFL, O_NONBLOCK) == -1)
		return (-1);
	return (0);
}

#endif
