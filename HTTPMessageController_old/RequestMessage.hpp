#ifndef REQUESTMESSAGE_HPP
# define REQUESTMESSAGE_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include "HTTPData.hpp"
#include "ErrorHandler.hpp"
#include "ConfigBlocks.hpp"
#include "CGIProcess.hpp"
#include "FileController.hpp"

extern NginxConfig::GlobalConfig
	_config;

class RequestMessage {
	public:
		typedef enum	e_Seq {
			START_LINE,
			HEADER_FIELD,
			MESSAGE_BODY,
			FINISH_PARSE,
			ERROR
		}				Seq;
		std::string error_code;

	private:
		HTTPData*	data;
		int			parsing_pointer;
		std::string	message;
		Seq			seq;
		bool		has_index;

	public:
		RequestMessage(HTTPData* _data);

		void						setMessage(char* buffer);
		void						resetMessage();
		std::string					getMessage();

		/** Header **/
		int							parsingRequestMessage();
		int							parseStartLine(std::string &msg);
		int							parseMethod(int &start, int &end, std::string &msg);
		int							parseTarget(int &start, int &end, std::string &msg);
		int							checkAutoIndex(std::string _root, std::string _pth);
		void						checkTarget(void);
		std::vector<std::string>	checkURIDIR(void);
		std::vector<std::string>	checkErrorPage(void);
		std::string					getErrorPage(std::vector<std::string> error_page, std::string root);
		void						parseHttpVersion (int &start, int &end, std::string &msg);
		void						printStartLine(void);

		/** Header Field **/
		int							parseHeaderField(std::string &msg);
		void						printHeaderField(void);

		/** Body **/
		void						parseMessageBody(std::string &msg);
		void						printBody(void);
		void						set413ErrorPage(void);

		int							setError(int);
		void						checkIsCgi(void);
};

#endif
