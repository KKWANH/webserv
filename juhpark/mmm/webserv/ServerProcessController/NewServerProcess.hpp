/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NewServerProcess.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juhpark <juhpark@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/26 14:39:35 by juhpark           #+#    #+#             */
/*   Updated: 2022/02/26 23:06:14 by juhpark          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEWSERVERPROCESS_HPP
# define NEWSERVERPROCESS_HPP

#include "SocketController.hpp"
#include "KernelQueueController.hpp"
#include "ConfigController.hpp"
#include "ResponseMessageController.hpp"
//#include "ServerConnect.hpp"
//#include "ServerAccess.hpp"
//#include "ServerConnectProcess"
#include <dirent.h>
#include <sys/stat.h>

extern ConfigController config

//클래스는 2개
//TCP연결까지
//이후 절차들은 connect
//근데 만들면서 굳이 TCP연결하고 받아들이는걸 클래스로 나누는것 보다 거기까지 여기의 일부분으로 냅두는게
//더 편하지 않을까 생각이 듦

//connect에선 여러단계가 있다



class newServerProcess
{

	static int NewServerProcess(SocketController* Socket, KernelQueueController* Kqueue) {
		while (1)
		{
			Kqueue->setPollingCount(
					kevent(Kqueue->getKq(), Kqueue->getChangeList(), Kqueue->getChangeCount(),
						Kqueue->getEventList(), BUFSIZ, NULL));
			
			Kqueue->clearChangeList();
			Kqueue->resetChangeList();

			for (int i = 0; i < Kqueue->getPollingCount(); i++)
			{						// server read
						if (Kqueue->getEventList(i)->filter == EVFILT_READ 
								&& (int)Kqueue->getEventList(i)->ident == Socket->getSocketServer()) {
							Socket->setSocketClient
								(accept(Socket->getSocketServer(), Socket->getConvertedAddressClient(), Socket->getSocketLength()));
							fcntl(Socket->getSocketClient(), F_SETFL, O_NONBLOCK);
							Kqueue->setReadKqueue(Socket->getSocketClient(), NULL);
							std::cout << "Server connect : [" << Socket->getSocketClient() << "]" << std::endl;
							//그리고 여기서 conection클래스를 만든다
						}//여기까지가 ACCEPT
						//이제 여기서부터 connect클래스가 있을 곳
						//거기서 말했던게
						//1. request READ
						//2. Start line
						//3 Method
						//4 is CGI
						//5. Header_Field
						//6. CGI_RUN
						//7 CGI ENVP SET
						//8 request msg body
						//9 make response
						//->여기까지가 read이벤트의 영역
						//10 responsewrite
						//11 close
						//->여기까지가 write이벤트의 영역	
						//
						//else if (객체가 있을 경우 혹은 캐스팅 연산자로 )
						//{
						// ->근데 그냥 요 부분을 switch문으로 하는게 낫나
						//	if (request read단계)
						//	{
						//		여기서 스타트라인이랑 METHOD, CGI를 판별
						//		헤더있다면 헤더까지
						//		->헤더까지 길어봤자 1메가 넘겠나..?
						//		->그러고 CGI가 있다면 CGI_RUN에서 멈추고 아니면
						//		->메세지 바디쪽을 살핀다
						//		->다되었음 받은 메세지를 출력
						//	}
						//	else if (CGI가 있다)
						//	{
						//		CGI 관련 처리
						//	}
						//	else if ()
						//	{
						//		바디 파싱
						//	}
						//	else if ()
						//	{
						//		메세지 작성 
						//	}
						//
						//
						//	else if (10이라면)
						//	{
						//		적고
						//		close
						//	}
						//}
						

						//근데 다른 레퍼런스를 보니깐
						//복사랑, 함수출력을 switch/case문으로 해서 나온게 있더라고
						//->뭔가 깔쌈해보여

						//요 아래는 새로운 예시
						/*
						 * 	if (events) {
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
						*/


			}//이벤트 발생시 for
		}//큰 while
	}//process
};//class




#endif

