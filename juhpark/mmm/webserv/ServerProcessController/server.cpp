#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>
#include <vector>
#include <map>

class Kqueue {
	int kfd;
	struct timespec pollingTime;
	size_t get_events_size;
	size_t set_events_size;
	struct kevent* getEvent;
	struct kevent* setEvent;
public:
	Kqueue(size_t events_size) : get_events_size(events_size), set_events_size(0) {
		// polling time = 1 sec
		pollingTime.tv_sec = 1;
		pollingTime.tv_nsec = 0;
		getEvent = new struct kevent[events_size];
		setEvent = new struct kevent[events_size];
		kfd = kqueue();
	}
	~Kqueue() {
		delete getEvent;
		delete setEvent;
		close(kfd);
	}
	size_t accessEvents() {
		size_t events = kevent(kfd, setEvent, set_events_size, getEvent, get_events_size, &pollingTime);
		set_events_size = 0;
		return events;
	}
	void addEvent(int fd, int16_t event, void* instancePointer) {
		EV_SET(&setEvent[set_events_size++], fd, event, EV_ADD | EV_EOF, 0, 0, instancePointer);
	}

	void removeEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_DELETE, 0, 0, instancePointer);
	}

	void enableEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_ENABLE, 0, 0, instancePointer);
	}

	void disableEvent(int fd, int16_t event, void* instancePointer) {
	    EV_SET(&setEvent[set_events_size++], fd, event, EV_DISABLE, 0, 0, instancePointer);
	}

	void* getInstanceByEventIndex(int index) {
		std::cout << "YEAH" << std::endl;
		return getEvent[index].udata;
	}

	int getFdByEventIndex(int index) {
		return getEvent[index].ident;
	}

	bool isCloseByEventIndex(int index) {
    	return (getEvent[index].flags & EV_EOF);
	}
};

class Process {
public:
	virtual int run() = 0;
};

class Binder : public Process {
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t addressSize;
	int serverSocket;

public:
	Binder(int port) {
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);
		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	}
	~Binder() {
		close(serverSocket);
	}

	int gen() {
		int e1 = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
		int e2 = listen(serverSocket, 42);
		int e3 = fcntl(serverSocket, F_SETFL, O_NONBLOCK);
		if (e1 < 0 || e2 < 0 || e3 < 0) {
			std::cout << "바인드 에러~" << std::endl;
			exit(1);
		}
		return serverSocket;
	}

	int run() {
		// clientAddress를 이용해 요청한 클라이언트의 IP와 포트를 알 수 있음.
		return accept(serverSocket, (struct sockaddr *) &clientAddress, &addressSize);
	}
};

class HTTPConnection : public Process {
public:
    typedef enum e_Seq {REQUEST, RESPONSE, CLOSE, END} Seq;
    //typedef enum e_Seq {
	//REQUEST,
	//REQUEST_START,
	//REQUEST_METHOD,
	//REQUEST_CGI,
	//REQUEST_HEADER,
	//CGI_RUN,
	//REQUEST_BODY, 
	//} Seq;
private:
    Seq seq;
	int socket_fd;
	int cgi_fd; // not used
public:
	HTTPConnection(int fd) : socket_fd(fd), seq(REQUEST) {}
	~HTTPConnection() {
		close(socket_fd);
	}

	int gen() {
		return 0;
	}

	int run() {
		if (seq == REQUEST) {
			char buffer[1024];
			int readLength = read(socket_fd, buffer, 1024);
			std::cout << "data : " << std::string(buffer, readLength) << std::endl;
			// Request 시퀀스가 완료되면 다음 시퀀스로 넘어가게 해야 합니다...
			seq = RESPONSE;
		} else if (seq == RESPONSE) {
			std::string httpTestHeaderString;
			httpTestHeaderString += "HTTP/1.1 200 OK\r\n";
			httpTestHeaderString += "Content-Type: text/html\r\n";
			httpTestHeaderString += "\r\n";
			httpTestHeaderString += "<html>";
			httpTestHeaderString += "<head><title>hi</title></head>";
			httpTestHeaderString += "<body>";
			httpTestHeaderString += "<b><center> HI! </center></b>";
			httpTestHeaderString += "</body>";
			httpTestHeaderString += "</html>";
			write(socket_fd, httpTestHeaderString.data(), httpTestHeaderString.length());
			// Response 시퀀스가 완료되면 다음 시퀀스로 넘어가게 해야 합니다...
			seq = CLOSE;
		} else if (seq == CLOSE) {
			seq = END;
		}
		return seq; // 상호참조를 줄이기 위해 저는 클래스의 외부에서 커널큐 상태를 변경하는 것을 선호합니다.
	}
};


int main() {

	int clientSocket;
	char buffer[1024];
	std::string httpTestHeaderString;
	int readLength;

	Kqueue kq(100);

	Binder b1(8080);
	kq.addEvent(b1.gen(), EVFILT_READ, &b1);

	while (true) {
		int events = kq.accessEvents();
		if (events) {
			std::cout << "for start" << std::endl;
			for (int i = 0; i < events; i++) {
				Process* udata = reinterpret_cast<Process*>(kq.getInstanceByEventIndex(i));

				// 바인딩 처리
				if (dynamic_cast<Binder*>(udata) != NULL) {
                    Binder* binder = reinterpret_cast<Binder*>(udata);
                    int conn_socket = binder->run();
					std::cout << "conn_socket : " << conn_socket << std::endl;

					HTTPConnection* httpconnecion = new HTTPConnection(conn_socket);
					kq.addEvent(conn_socket, EVFILT_READ, httpconnecion);
					//아마 이쪽이 그때 말한 Accept클래스
					//
                }
                // HTTPConnection 처리
                else if (dynamic_cast<HTTPConnection*>(udata) != NULL) {
                    HTTPConnection* hc = reinterpret_cast<HTTPConnection*>(udata);
                    int result = hc->run();
					std::cout << "result : " << result << std::endl;
                    if (kq.isCloseByEventIndex(i)) {
                    	int fd = kq.getFdByEventIndex(i);
					    delete hc;
                    } else if (result == HTTPConnection::RESPONSE) {
                    	// READ -> WRITE
                    	int fd = kq.getFdByEventIndex(i);
						std::cout << "kq(r) : " << fd << std::endl;
					    kq.disableEvent(fd, EVFILT_READ, udata);
					    kq.enableEvent(fd, EVFILT_WRITE, udata);
                    } else if (result == HTTPConnection::CLOSE) {
                    	// 이벤트 제거
                    	int fd = kq.getFdByEventIndex(i);
						std::cout << "kq(w) : " << fd << std::endl;
					    kq.removeEvent(fd, EVFILT_WRITE, udata);
                    } else {
                    	// kevent를 한번만 사용하기 때문에 이전 Write가 한번 더 들어와서 이 때 close 처리를 해야 함.
					    delete hc;
                    }
                }
			}
		} else {
			std::cout << "waiting..." << std::endl;
		}
	}

	return (0);
}
