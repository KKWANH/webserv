#ifndef SERVERCONNECTPROCESS
# define SERVERCONNECTPROCESS

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
class ServerConnectProcess
{
};//클래스

#endif 
