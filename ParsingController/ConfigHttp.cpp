# include				"ConfigBlocks.hpp"

NginxConfig::HttpBlock::HttpBlock(
		std::string	_str)
	:	NginxBlock(_str)
{
	setDirectiveTypes();
	setBlock();
	checkHttpBlock();
}

void
	NginxConfig::HttpBlock::setDirectiveTypes(
		void)
{
	_dir_case.push_back("charset");
	_dir_case.push_back("default_type");
	_dir_case.push_back("keepalive_timeout");
	_dir_case.push_back("sendfiile");
	_dir_case.push_back("types");
	_dir_case.push_back("server");
}

void
	NginxConfig::HttpBlock::setBlock(
		void)
{
	std::size_t
		_pos = 0,
		_block_pos = 0;
	bool
		_types_only_one_flag = true;
	
	while (_raw_data[_pos])
	{
		std::string
			_tmp_line = Parser::getIdentifier(_raw_data, _pos, "\n", false);
		
		if (Parser::sideSpaceTrim(_tmp_line).empty())
			continue;
		
		std::size_t
			_tmp_pos = 0;
		std::string
			_tmp_dir = Parser::getIdentifier(_tmp_line, _tmp_pos, " ", true);
		if (find(_dir_case.begin(), _dir_case.end(), _tmp_dir) == _dir_case.end())
			continue;
		else if (_tmp_dir == "server")
		{
			ServerBlock
				_tmp_server_block(NginxParser::getBlockContent(_raw_data, _block_pos));
			_server.push_back(_tmp_server_block);
			_pos = _block_pos;
		}
		else if (!_dir_map[_tmp_dir].empty())
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				_tmp_dir + " in http context is not empty");
		else if (_tmp_dir == "types")
		{
			if (!_types_only_one_flag)
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"Duplicate type context: " + _tmp_dir + " in http context");
			_types_only_one_flag = false;
			TypesBlock
				_tmp_types_block(NginxParser::getBlockContent(_raw_data, _block_pos));
			_types = _tmp_types_block;
			_pos = _block_pos;
		}
		else
		{
			std::string
				_tmp_val = Parser::sideSpaceTrim(Parser::getIdentifier(_tmp_line, _tmp_pos, ";", true));
			std::vector<std::string>
				_tmp_split = Parser::getSplitBySpace(_tmp_val);
			if (_tmp_split.size() != 1)
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"Invalid number of arguments in http[" + _tmp_dir + " directive]");
			_dir_map[_tmp_dir] = _tmp_split[0];
		}
	}
}

void
	NginxConfig::HttpBlock::checkHttpBlock(
		void)
{
	std::vector<ServerBlock>::iterator
		_itr;
	std::vector<std::string>
		_tmp_listen;
	
	for (_itr = _server.begin(); _itr != _server.end(); ++_itr)
		_tmp_listen.push_back(_itr->_dir_map["listen"]);
	if (!(std::unique(_tmp_listen.begin(), _tmp_listen.end()) == _tmp_listen.end()))
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"repeated listening port in different server context");
}