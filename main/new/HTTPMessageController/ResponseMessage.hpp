#ifndef RESPONSEMESSAGE_HPP
# define RESPONSEMESSAGE_HPP

#include "HTTPData.hpp"

class ResponseMessage {
	private:
		/**
		 * data : Request Message로부터 파싱된 데이터들이 담긴 클래스
		 * message : Response message (start line + header field + message body)
		 * statusMessagePath : 상태코드에 따른 메세지를 설정하기 위한 파일 경로
		*/
		HTTPData*		data;
		std::string		message;
		std::string		start_line;
		std::string		header_field;
		std::string		message_body;
		std::string		statusMessagePath;
	public:
		/**
		 * setStartLine -> setMessageBody -> setHeaderField 순으로 호출
		 * message body가 있어야 header field의 content-length를 구할 수 있기 때문
		*/
		ResponseMessage(HTTPData* _data);
		void		setStartLine();
		std::string	setStatusMessage(std::string status_code);
		void		setHeaderField();
		void		setMessageBody();
		void		setResponseMessage(std::string _tmp_directory);

		void		printStartLine();
		void		printHeaderField();
		void		printMessageBody();
		std::string	getMessage();

		void		resetMessage(int buf_size);
};

#endif
