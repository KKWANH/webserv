#ifndef HTTPCONNECTION_HPP
# define HTTPCONNECTION_HPP

#include "../Process.hpp"
#include "HTTPData.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"
#include "SocketController.hpp"
#include "CGIProcess.hpp"

class HTTPConnection : public Process {
	public:
		typedef enum e_Status {
			REQUEST,
			CGI,
			RESPONSE,
			CLOSE,
			END
		} Status;

	private:
		Status 				status;
		HTTPData			data;
		RequestMessage		requestMsg;
		ResponseMessage		responseMsg;
		SocketController	socket;
		CGIProcess			cgi;
		
	public:
		HTTPConnection(SocketController socket) : socket(socket), status(REQUEST) {}
		~HTTPConnection() {
			close(socket.getSocketServer());
		}

		int		run(void) {
			if (status == REQUEST) {
				// Request Msg 읽기
				// 필요한 데이터 저장
				// requestMsg 안에서 cgi가 동작이 되도록
			} else if (status == RESPONSE) {
				// 저장된 데이터를 토래도 responseMsg 에 작성

				/** joopark님 질문 : 두번의 과정을 거쳐서 close -> end 를 한 이유는 무엇인가요? **/
				/** 대답 :  **/
				//저희 과제에서 kevent를 한 번만 호출해야 하기 때문에 데이터를 다 쓰고 나서 Close 처리를 할 때
				// close 처리를 할 때 write 이벤트가 한번 더 들어와서 이렇게 해야 합니다...!
				// close 시퀀스에서 close 처리를 하면 바로 반영되는게아니라 지연이 된다고 해야 하나...?? 그래서 그렇습니다
				// kq.removeEvent(fd, EVFILT_WRITE, udata); 이런식으로 이벤트 제거하기 위해 커널큐에 이벤트 삽입할 때
				// 다음 kevent 호출 시점에 이미 write 이벤트(무시해도 되는 이벤트)가 들어와 있어요 그래서 두번의 과정을 거쳤습니다
				// 추가 설명이 필요하시면 슬랙콜 통해 리뷰 드리겠습니다...!

				status = CLOSE;
			} else if (status == CLOSE) {
				status = END;
			}
				// status가 END인 경우는,run 함수 밖에서 체크할 수 있도록 합니다.

			return status;
		}
};

#endif
