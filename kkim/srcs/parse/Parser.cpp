# include				"Parser.hpp"
Parser::Parser(				const std::string&		_str)
{
	std::ifstream
		_read_file;
	this->_raw_data = "";

	_read_file.open(this->_file_name);
	if (!_read_file.is_open())
		throw ErrorHandler(__FILE__, __func__, __LINE__, "configuration file open error");
	
	while (!_read_file.eof())
	{
		std::string
			_tmp;
		getline(_read_file, _tmp);
		this->_raw_data += _tmp;
		this->_raw_data += "\n";
	}

	_read_file.clear();

	if (!isValidBlockSet(this->_raw_data))
		throw ErrorHandler(__FILE__, __func__, __LINE__, "bracket pair is not matched");
}

Parser::~Parser(void) {}

const std::string&
	Parser::getRawData(void) const
{
	return (this->_raw_data);
}

bool
	Parser::isValidBlockSet(const std::string&		_buf)
{
	std::size_t
		_idx = 0;
	int
		_left = 0,
		_right = 0;

	while(_buf[_idx] == '{')
	{
		if (_buf[_idx] == '{')
			++_left;
		else
			++_right;
		++_idx;
	}

	return _left == _right;
}

std::string
	Parser::leftSpaceTrim(	std::string				_str)
{
	const	std::string _drp = " ";
	return	_str.erase(0, _str.find_first_not_of(_drp));
}

std::string
	Parser::rightSpaceTrim(	std::string				_str)
{
	const	std::string	_drp = " ";
	return	_str.erase(_str.find_last_not_of(_drp) + 1);
}

std::string
	Parser::sideSpaceTrim(	std::string				_str)
{
	const std::string	_drp	= " ";
	std::string			_rst	= _str.erase(_str.find_last_not_of(_drp) + 1);
	_rst						= _rst.erase(0, _rst.find_first_not_of(_drp));
	return						_rst;
}

bool
	Parser::isCharInString(	const std::string&		_str,
							const char				_chr)
{
	std::size_t		_idx = 0;

	while (_str[_idx])
	{
		if (_str[_idx] == _chr)
			return	true;
		++_idx;
	}
	return			false;
}

std::string
	Parser::getIdentifier(	const std::string		_str,
							std::size_t&			_end_pos,
							std::string				_delimiter,
							bool					_checker)
{
	size_t			_word_size = 0;

	if (_checker && _str.find(_delimiter) == std::string::npos)
		throw ErrorHandler(__FILE__, __func__, __LINE__, "delimiter is not found");

	while ((_str[_end_pos] != '\0') && isCharInString(_delimiter, _str[_end_pos]))
		++_end_pos;
	
	size_t			_stt_pos = _end_pos;
	while ((_str[_end_pos] != '\0') && !isCharInString(_delimiter, _str[_end_pos]))
	{
		++_end_pos;
		++_word_size;
	}

	return			_str.substr(_stt_pos, _word_size);
}

std::vector<std::string>
	Parser::getSplitBySpace(std::string				_str)
{
	std::vector<std::string>
		_rst;
	std::size_t
		_idx = 0;
	
	while (_idx < _str.size())
	{
		std::string
			_tmp = getIdentifier(_str, _idx, " ", false);
		if (_tmp.empty())
			break ;
		_rst.push_back(_tmp);
	}

	return _rst;
}

bool
	Parser::isNumber(		const std::string&		_str)
{
	int
		_idx = 0;
	
	while (_str[_idx])
	{
		if (!isdigit(_str[_idx]))
			return	false;
		++_idx;
	}
	return	true;
}
