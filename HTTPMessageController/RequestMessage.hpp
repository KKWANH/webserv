#ifndef		REQUESTMESSAGE_HPP
# define	REQEUSTMESSAGE_HPP

# include	<iostream>
# include	<unistd.h>
# include	<cstring>
# include	"HTTPData.hpp"
# include	"ErrorHandler.hpp"
# include	"ConfigBlocks.hpp"
# include	"CGIProcess.hpp"
# include	"FileController.hpp"

extern NginxConfig::GlobalConfig
	_config;

class RequestMessage
{
	public:
		typedef enum e_seq {
			START_LINE,
			HEADER_FIELD,
			MESSAGE_BODY,
			FINISH_PARSE,
			CGI,
			AUTOINDEX,
			REDIRECT,
			ERROR
		} e_seq;
	
	private:
		HTTPData*
			_data;
		int
			_parse_ptr;
		std::string
			_message;
		e_seq
			_seq;
		bool
			_has_index;
	
	public:
		// Contructor
		RequestMessage(
				HTTPData* _tmp);
		
		// Setter && Getter
		int
			setError(
				int _arg_status_code);
		void
			setMessage(
				char* buffer);
	
		std::string
			getMessage(
				void);
		
		int
			parse(
				void);
		
		// Header
		void
			parseStartLine(
				std::string& msg);
		void
			parseMethod(
				int& start,
				int& end,
				std::string& msg);
		void
			parseTarget(
				int& start,
				int& end,
				std::string& msg);

		// Header Field
		int
			parseHeaderField(
				std::string& msg);
		
		// Body
		void
			parseMessageBody(
				std::string& msg);
};

#endif