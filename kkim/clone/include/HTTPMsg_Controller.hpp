// #ifndef					HTTPMSG_CONTROLLER_HPP
// # define				HTTPMSG_CONTROLLER_HPP

// # include				<string>
// # include				<map>
// # include				<iostream>
// # include				<sys/socket.h>
// # include				<arpa/inet.h>
// # include				<unistd.h>
// # include				<utility>
// # include				<sstream>
// # include				<fstream>
// # include				<dirent.h>
// # include				<sys/stat.h>

// # include				"Utils.hpp"
// # include				"Server_Process.hpp"
// # include				"Config_Controller.hpp"

// extern ConfigController	_cnf(false);
// extern ConfigController	_mim(true);

// class					HTTPMsg
// {
// 	protected:
// 		std::map<std::string, std::string>
// 			_fld_head;
// 		std::string
// 			_stt_line;
// 		std::string
// 			_msg_body;
	
// 	public:
// 		std::string		getMessageBody(void)	{ return (_msg_body); }
// 		std::string		getStartLine(void)		{ return (_stt_line); }

// 		// TODO: body-message가 있을 경우를 대비하여 CRLF 이후의 위치를 반환하도록 수정
// 		// header_field를 파싱하여 map에 저장하는 함수
// 		// message : 클라이언트 측에서 전송하는 데이터
// 		// pos : start_line을 parsing하고 난 이후의 위치
// 		void			parseHeaderField(std::string _msg, int &_pos)
// 		{
// 			int			_stt = _pos, _lst;
// 			std::string	_key, _val;

// 			// if header_field doesn't exist
// 		}

// }