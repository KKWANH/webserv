#include "./test_util.hpp"

using namespace std;

// kqueue에 set할 데이터가 들어있는 구조체 kevent를 clear
void		clearChangeList(int remove_count, struct kevent* change_list) {
	for (int i = 0; i < remove_count; i++) {
		EV_SET(&change_list[i], 0, 0, 0, 0, 0, NULL);
	}
	return ;
}

int			main(void) {

	// Variables for Socket communication
	int									c_socket, s_socket;
	struct sockaddr_in	s_addr, c_addr;
	socklen_t						len;

	// Variables for I/O Multiplexing with function 'kqueue'
	int							kq;										// kqueue descriptor
	struct kevent		event_list[BUFSIZ];		// 작업을 돌려받는 list
	struct kevent		change_list[BUFSIZ];	// 작업을 추가하기 위한 list
	int							change_count;					// 추가할 작업 수
	std::string			message_data[BUFSIZ];	// 통신 시 주고받는 데이터를 임시로 저장할 장소
	int							polling_count;				// event 수

	// Initialize settings about socket communication
	if (initSocket(&s_socket, &s_addr) == -1)
		return (-1);
	// Initialize settings about I/O Multiplexing	
	if (initKqueue(&kq, &change_count, s_socket, change_list) == -1)
		return (-1);
	
	while (true) {
		// get num of event
		polling_count = kevent(kq, change_list, change_count, event_list, 10, NULL);
		// clear chagne_list
		clearChangeList(change_count, change_list);
		change_count = 0;

		for (int i = 0; i < polling_count; i++) {
			// READ Request
			if (event_list[i].filter == EVFILT_READ) {
				// server
				if (event_list[i].ident == s_socket) {
					c_socket = accept(s_socket, (struct sockaddr*)&c_addr, &len);
					cout << "server-client connect with fd[" << c_socket << "]" << endl;
					fcntl(c_socket, F_SETFL, O_NONBLOCK);
					// set fd about client
					EV_SET(&change_list[0], c_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					EV_SET(&change_list[1], c_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					message_data[event_list[i].ident] = "";
					change_count++;
					change_count++;
				}
				// client
				else {
					char buf[BUFSIZ];
					recv(event_list[i].ident, buf, BUFSIZ, 0);
					message_data[event_list[i].ident] = buf;
				}
			}
			// WRITE Request
			else if (event_list[i].filter == EVFILT_WRITE) {
				if (message_data[event_list[i].ident] != "") {
					write(event_list[i].ident, message_data[event_list[i].ident].c_str(), message_data[event_list[i].ident].size());
					cout << "fd[" << event_list[i].ident << "] : " << message_data[event_list[i].ident] << endl;
					message_data[event_list[i].ident] = "";
					close(event_list[i].ident);
				}
			}
		}
	}
	close(s_socket);
	return (0);
}

