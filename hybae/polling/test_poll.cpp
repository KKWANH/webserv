#include "./test_util.hpp"

using namespace std;

int			main(void) {

	// Variables for Socket communication
	int									c_socket, s_socket;
	struct sockaddr_in	s_addr, c_addr;
	socklen_t						len;

	// Variables for I/O Multiplexing with function 'poll'
	struct pollfd 			poll_fd[BUFSIZ];
	int									polling_count;

	// Initialize settings about socket communication
	if (initSocket(&s_socket, &s_addr) == -1)
		return (-1);
	// Initialize settings about I/O Multiplexing	
	if (initPoll(poll_fd, s_socket) == -1)
		return (-1);

	while (true) {
		// get num of event
		polling_count = poll(poll_fd, BUFSIZ, -1);

		// server
		if (poll_fd[0].revents == POLLIN) {
			c_socket = accept(s_socket, (struct sockaddr*)&c_addr, &len);
			// set new fd
			for (int i = 0; i < BUFSIZ; i++) {
				if (poll_fd[i].fd == -1) {
					poll_fd[i].fd = c_socket;
					poll_fd[i].events = POLLIN;
					poll_fd[i].revents = 0;
					break;
				}
			}
			cout << "server-client connect with fd[" << c_socket << "]" << endl;
		}
		// client
		for (int i = 1; i < BUFSIZ; i++) {
			switch (poll_fd[i].revents) {
				// skip about request to server
				case 0:
					break;
				case POLLIN:
					char buf[BUFSIZ];
					recv(poll_fd[i].fd, buf, BUFSIZ, 0);
					write(poll_fd[i].fd, buf, BUFSIZ);
					cout << "fd[" << poll_fd[i].fd << "] : " << buf << endl;
				default:
					close(poll_fd[i].fd);
					poll_fd[i].fd = -1;
					poll_fd[i].revents = 0;
					break;
			}
		}
	}
	close(s_socket);
	return (0);
}