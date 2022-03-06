#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

class HTTPConnection : public ClassController {
	public:
		typedef enum	e_Seq {
			REQUEST,
			RESPONSE,
			CLOSE,
			END
		}				Seq;

	private:
		Seq seq;
		int socket_fd;
		//int cgi_fd; // not used
		
	public:
		HTTPConnection(int fd) : seq(REQUEST), socket_fd(fd) {}
		virtual ~HTTPConnection() {
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

#endif
