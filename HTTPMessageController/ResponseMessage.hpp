#ifndef RESPONSEMESSAGE_HPP
# define RESPONSEMESSAGE_HPP

# include	"HTTPData.hpp"
# include	<fstream>
# include	<sstream>
# include	"ConfigBlocks.hpp"
# include	"ConfigMime.hpp"
# include	"FileController.hpp"

extern MimeConfig
	_mime;
extern NginxConfig::GlobalConfig
	_config;

class ResponseMessage {
	private:
		/**
		 * data : Request Message로부터 파싱된 데이터들이 담긴 클래스
		 * message : Response message (start line + header field + message body)
		 * statusMessagePath : 상태코드에 따른 메세지를 설정하기 위한 파일 경로
		*/
		HTTPData*
			_data;
		std::string
			_message,
			_start_line,
			_header_field,
			_status_msg_path;
	public:
		/**
		 * setStartLine -> setMessageBody -> setHeaderField 순으로 호출
		 * message body가 있어야 header field의 content-length를 구할 수 있기 때문
		*/
		FileController::Type
				_type;
		ResponseMessage(HTTPData* _data);
		std::string
			returnRedirectMessage(void);
		void
			setStartLine(void);
		std::string
			setStatusMessage(std::string status_code);
		void
			setHeaderField(void);
		int
			setResponseMessage(void);

		void
			printStartLine(void);
		void
			printHeaderField(void);

		std::string
			getMessage();
		void
			resetMessage(int buf_size);
};

#endif
