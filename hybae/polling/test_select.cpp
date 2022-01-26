#include "./test_util.hpp"

using namespace std;

int			main(void) {

	// Variables for Socket communication
	int									c_socket, s_socket;
	struct sockaddr_in	s_addr, c_addr;
	socklen_t						len;

	// Variables for I/O Multiplexing with function 'select'
	struct timeval			timeout;
	fd_set							read, cpy_read;
	int									fd_max = 0, fd_num = 0;

	// Initialize settings about socket communication
	if (initSocket(&s_socket, &s_addr) == -1)
		return (-1);

	// Initialize settings about I/O Multiplexing
	if (initSelect(&read, &fd_max, s_socket, &timeout) == -1)
		return (-1);

	while (true) {
		cpy_read = read;
		// get num of fd in cpy_read
		fd_num = select(fd_max + 1, &cpy_read, 0, 0, &timeout);

		// error
		if (fd_num == -1)
			return (-1);
		// timeout
		if (fd_num == 0)
			continue;

		// fd 순회
		for (int fd = 0; fd < fd_max + 1; ++fd) {
			// cpy_read 내 fd 있는 경우
			if (FD_ISSET(fd, &cpy_read)) {
				// server
				if (fd == s_socket) {
					len = sizeof(c_addr);
					c_socket = accept(s_socket, (struct sockaddr*)&c_addr, &len);
					// add fd(c_socket) to read
					FD_SET(c_socket, &read);
					// increase fd_max
					fd_max++;
					cout << "server-client connect with fd[" << c_socket << "]" << endl;
				}
				// client
				else {
					char buf[BUFSIZ];
					recv(fd, buf, BUFSIZ, 0);
					write(fd, buf, BUFSIZ);
					cout << "fd[" << fd << "] : " << buf << endl;
					// remove fd from read
					FD_CLR(fd, &read);
					// decrease fd_max
					fd_max--;
					close(fd);
				}
			}
		}
	}
	close(s_socket);
	return (0);
}