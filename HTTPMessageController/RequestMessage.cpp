# include	"RequestMessage.hpp"

// Contructor

RequestMessage::RequestMessage(
		HTTPData* _tmp)
{
	this->_data = _tmp;
	this->_parse_ptr = 0;
	this->_message = "";
	this->_seq = START_LINE;
	this->_has_index = false;
	this->_status_code = 200;
}

// Getter && Setter

void
	RequestMessage::setMessage(
		char* buffer)
{
	std::string	temp(buffer);
	this->_message += temp;
	return ;
}

std::string	
	RequestMessage::getMessage(
		void)
{
	return (this->_message);
}

int
	RequestMessage::setError(
		int _arg_status_code)
{
	this->_data->status_code = _arg_status_code;
	this->_seq = ERROR;
	return (ERROR);
}

// parse main

int
	RequestMessage::parse(
		void)
{
	int
		_rst;

	if (this->_seq == START_LINE)
	{
		if (int(this->_message.find("\r\n")) != -1)
		{
			_rst = parseStartLine(this->_message);
			if (_rst == ERROR)
			{
				
			}
			else if (_rst == IS_CGI)
			{

			}
			else if (_rst == IS_AUTOINDEX)
			{
				// if (this->seq != ERROR && this->has_index == false)
				// {
				// 	if (checkAutoIndex(
				// 			_config._http._server[1]._dir_map["root"],
				// 			this->data->url_directory)
				// 		== 1)
				// 	{
				// 		data->is_autoindex = true;
				// 		data->status_code = 200;
				// 	}
				// }
			}
			else if (_rst == IS_REDIRECT)
			{
				
			}
			this->_seq == HEADER_FIELD;
		}
	}
	else if (this->_seq == HEADER_FIELD)
	{

	}
	else if (this->_seq == MESSAGE_BODY)
	{

	}
	return (this->_seq);
}

// Start Line
int
	RequestMessage::parseStartLine(
		std::string& msg)
{
	int
		_start,
		_end;

	parseMethod(_start, _end, msg);
	if (this->_seq != ERROR)
	{
		parseTarget(_start, _end, msg);
		if (this->_seq != ERROR)
		{
			parseHttpVersion(_start, _end, msg);
			if (this->_seq != ERROR)
			{
				if (this->_has_index == false &&
						checkAutoindex(
						_config._http._server[1]._dir_map["root"],
						this->_data->url_directory) == 1)
					this->_seq = AUTOINDEX;
			}
			return (this->_seq);
		}
	}
}

// 405: Method Not Allowed

void
	RequestMessage::parseMethod(
		int& _start,
		int& _end,
		std::string& _msg)
{
	_start = 0;
	_end = _msg.find(' ');
	if (_end == -1)
		this->_seq = this->setError(400);
	_data->method = _msg.substr(_start, _end);
	if (_data->method.compare("GET") != 0 &&
		_data->method.compare("POST") != 0 &&
		_data->method.compare("DELETE") != 0)
		this->_seq = this->setError(405);
	/*
	if (nginx.conf 내 accepted_method가 있는 경우) {
		if (현재 HTTP method가 accepted_method와 매칭되지 않는 경우)
			return (setError(405));
	}
	*/
	this->_seq = START_LINE;
}