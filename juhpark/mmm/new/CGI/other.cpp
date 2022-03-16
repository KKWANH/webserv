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
#include <unistd.h>

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
		return getEvent[index].udata;
	}
	int getDataByEventIndex(int index) {
		return getEvent[index].data;
	}
	int getFdByEventIndex(int index) {
		return getEvent[index].ident;
	}
	bool isCloseByEventIndex(int index) {
    	return (getEvent[index].flags & EV_EOF);
	}
};


class CGISession {
    private:
        pid_t _pid;
        int _inputStream;
        int _outputStream;
        std::string _path;
        CGISession();
    public:
        CGISession(std::string path) : _path(path), _pid(-1) {}
        ~CGISession() {
        }
        int getInputStream(void) { return _inputStream; }
        int getOutputStream(void) { return _outputStream; }
        void run() {
		    int pairForI[2];
		    int pairForO[2];
		    if (pipe(pairForI) == -1 || pipe(pairForO) == -1) {
		    	std::cout << "파이프 생성 에러~" << std::endl;
		        exit(-1);
		    }
		    _inputStream = pairForI[1];
		    _outputStream = pairForO[0];
		    if ((_pid = fork()) < 0) {
		    	std::cout << "포크 에러~" << std::endl;
		        exit(-1);
		    }
		    if (_pid == 0) {
		        if ((dup2(pairForI[0], STDIN_FILENO) == -1) || (dup2(pairForO[1], STDOUT_FILENO) == -1)) {
		    		std::cout << "파이프 복제 에러~" << std::endl;
		        	exit(-1);
		        }
		        if ((close(pairForI[1]) == -1) || (close(pairForO[0]) == -1)) {
		    		std::cout << "파이프 닫기 에러~" << std::endl;
		        	exit(-1);
		        }
		        char* _arg[2];
		        _arg[0] = new char[8];
				strcpy(_arg[0], "/bin/cat");
		        _arg[1] = NULL;
		        if (execve(_path.c_str(), _arg, NULL) == -1) {
		    		std::cout << "프로세스 실행 에러~" << std::endl;
		        	exit(-1);
		        }
		    } else {
		        if ((close(pairForI[0]) == -1) || (close(pairForO[1]) == -1)) {
		    		std::cout << "파이프 닫기 에러~" << std::endl;
		        	exit(-1);
		        }
		    }
        }
};

int main(int argc, char const *argv[])
{
	Kqueue kq(100);
	
	int cnt = 10;
	
	CGISession cgi("/bin/cat"); // cat 명령어는 데이터를 표준입력으로 EOF를 보낸 만큼 표준출력으로 동일한 값을 출력해 줌.
	cgi.run(); //wait없이 지금 시전해도 되는 이유는 어차피 아래 while문에서 이벤트를 기다리고 있기 떄문이란다
	int cgi_write_fd = cgi.getInputStream();
	int cgi_read_fd = cgi.getOutputStream();
	
	kq.addEvent(cgi_write_fd, EVFILT_WRITE, NULL);
	while (true) {
		int events = kq.accessEvents();
		if (events) {
			for (int i = 0; i < events; i++) {
				std::cout << "fd : " << kq.getFdByEventIndex(i) << std::endl;
				std::cout << "data : " << kq.getDataByEventIndex(i) << std::endl;
				if (kq.isCloseByEventIndex(i)) { // 프로세스는 데이터를 다 전송하면 close()를 하기 때문에 close 이벤트로 감지할 수 있음.
					std::cout << "close fd " << kq.getFdByEventIndex(i) << std::endl;
					close(kq.getFdByEventIndex(i)); // Close 처리를 해야 커널큐에서 제거됨
				} else {
					// 보낼 때 데이터를 10번 끊어서 보냄.
					if (kq.getFdByEventIndex(i) == cgi_write_fd) {
						int writeLength = write(cgi_write_fd, "hello~", 6);
						std::cout << "writeLength : " << writeLength << std::endl;
						cnt--;
						if (cnt == 0) { // 데이터를 다 보내면 fd를 닫음.
							close(cgi_write_fd);
							kq.addEvent(cgi_read_fd, EVFILT_READ, NULL);
						}
					//write 요약
					//만약에 CGI부분에 적을 게 있다면
					//fd를 받아서 
					//이벤트를 wirte로 바꾸게 하여(이떄, fd는 파이프에서 가져온 Input fd)
					//write 를 시전한다
					//여기 예시로는 미리 카운트를 10주어서
					//한번 write할떄마다 1을 깎게 되고 0이 되었으면 해당 fd를 닫아버리고 read로 이벤트를 바꾸게 만든다
					} else {
						// 버퍼를 일부러 작은 값으로 설정해서 끊어서 받도록 유도함.
						char buffer[10];
						int readLength = read(cgi_read_fd, buffer, 10);
						std::cout << "readLength : " << readLength << std::endl;
						std::cout << "data : " << std::string(buffer, readLength) << std::endl;
					}
					//만약에 CGI쪽에서 다 읽고 read로 되었다면 읽은 fd를 기준으로 가져온다
				}
			}
		} else {
			std::cout << "waiting..." << std::endl;
		}
	}
	return 0;
}
