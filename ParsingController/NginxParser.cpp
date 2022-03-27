# include				"NginxParser.hpp"

NginxParser::NginxParser(void)
:	Parser()
{ }

void
	NginxParser::startNginxParser(
		const std::string& _str)
{
	Parser::setParserStr(_str);
}

void
	NginxParser::skipComment(	const std::string&	_str,
								std::size_t&		_comment_pos)
{
	while (_str[_comment_pos] == ' ')
		++_comment_pos;
	if (_str[_comment_pos] != '#')
		return ;
	while (_str[_comment_pos])
	{
		if (_str[_comment_pos] != '\n')
			++_comment_pos;
		else
			return ;
	}
}

void
	NginxParser::findBlockSet(	const std::string&	_buf,
								std::stack<int>& 	_stc,
								std::vector<std::pair<std::string, std::size_t> >&
													_vec,
								std::size_t&		_pos)
{
	while (!(_buf[_pos] == '\0' ||
			 _buf[_pos] == '{' ||
			 _buf[_pos] == '}'))
		++_pos;
	if (_buf[_pos] == '{')
	{
		_stc.push(1);
		_vec.push_back(std::make_pair("{", _pos));
	}
	else if (_buf[_pos] == '}')
	{
		if (_stc.top() == 1)
			_stc.pop();
		_vec.push_back(std::make_pair("}", _pos));
	}
	if (_stc.empty())
		return ;
	_pos += 1;
	findBlockSet(_buf, _stc, _vec, _pos);
}

std::string
	NginxParser::getBlockContent(
								const std::string&	_buf,
								std::size_t&		_pos)
{
	std::vector<std::pair<std::string, std::size_t> >
		_block_vector;
	std::stack<int>
		_stc;

	findBlockSet(_buf, _stc, _block_vector, _pos);

	if (_block_vector.empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__, "error in getBlockContent: bracket is empty");

	std::size_t
		_block_stt = _block_vector.begin()->second;
	std::size_t
		_block_end = (_block_vector.end() - 1)->second;

	_pos = _block_end + 1;

	return _buf.substr(_block_stt + 1, _block_end - _block_stt - 1);
}