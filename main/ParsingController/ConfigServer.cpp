# include				"ConfigBlocks.hpp"

NginxConfig::ServerBlock::ServerBlock(
		std::string	_str)
	:	NginxBlock(_str)
{
	this->setDirectiveTypes();
	this->setBlock();
	this->checkServerBlock();
}

void
	NginxConfig::ServerBlock::setDirectiveTypes(
		void)
{
	_dir_case.push_back("root");
	_dir_case.push_back("index");
	_dir_case.push_back("autoindex");
	_dir_case.push_back("error_page");
	_dir_case.push_back("listen");
	_dir_case.push_back("server_name");
	_dir_case.push_back("location");
	_dir_case.push_back("client_max_body_size");
	_dir_case.push_back("keepalive_timeout");
}

NginxConfig::InheritData
	NginxConfig::ServerBlock::getInheritData(
		void)
{
	InheritData
		_ihd;
	
	if (!_dir_map["root"].empty())
        _ihd._root = _dir_map["root"];
    if (!_dir_map["autoindex"].empty())
        _ihd._autoindex = _dir_map["autoindex"];
    if (!_index.empty())
        _ihd._index = _index;
    if (!_error_page.empty())
        _ihd._error_page = _error_page;
    if (!_dir_map["client_max_body_size"].empty())
        _ihd._client_max_body_size = _dir_map["client_max_body_size"];
    return _ihd;
}

void
	NginxConfig::ServerBlock::setBlock(
		void)
{
	std::string
		_buf = _raw_data;
	std::size_t
		_pos = 0,
		_block_pos = 0;
	
	while (_buf[_pos])
	{
		std::string
			_tmp_line = Parser::getIdentifier(_buf, _pos, "\n", false);
		if (Parser::sideSpaceTrim(_tmp_line).empty())
			continue ;
		
		std::size_t
			_tmp_pos = 0;
		std::string
			_tmp_dir = Parser::getIdentifier(_tmp_line, _tmp_pos, " ", true);
		
		if (find(_dir_case.begin(), _dir_case.end(), _tmp_dir) == _dir_case.end())
			throw ErrorHandler(__FILE__, __func__, __LINE__, 
				_tmp_dir + " is not in context[server] list");
		else if (_tmp_dir == "location")
		{
			InheritData
				_ihd = getInheritData();
			LocationBlock
				_tmp_loc_block(
					NginxParser::getBlockContent(_buf, _block_pos),
					Parser::sideSpaceTrim(
						Parser::getIdentifier(_tmp_line, _tmp_pos, "{", true)),
						_ihd);
			_location.push_back(_tmp_loc_block);
			_pos = _block_pos;
		}
		else if (!_dir_map[_tmp_dir].empty())
			throw ErrorHandler(__FILE__, __func__, __LINE__, 
				"Duplicate directive: " + _tmp_dir + " is already set");
		else
		{
			std::string
				_tmp_val = Parser::sideSpaceTrim(Parser::getIdentifier(_tmp_line, _tmp_pos, ";", true));
			if (_tmp_dir		== "index")
				_index			=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	== "root")
				_dir_map["root"]=	_tmp_val;
			else if (_tmp_dir	== "error_page")
				_error_page		=	Parser::getSplitBySpace(_tmp_val);
			else
			{
				std::vector<std::string>
					_tmp_split = Parser::getSplitBySpace(_tmp_val);
				if (_tmp_split.size() != 1)
					throw ErrorHandler(__FILE__, __func__, __LINE__, 
						"Invalid number of arguments: " + _tmp_dir + ": " + _tmp_val);
				_dir_map[_tmp_dir] = _tmp_split[0];
			}
		}
	}
}

void
	NginxConfig::ServerBlock::checkServerBlock(
		void)
{
	if (_location.empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__, 
			"No location block in server block");
	if (_dir_map["listen"].empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"No port number for listening");
	if (_dir_map["root"].empty())
		_dir_map["root"] = DEFAULT_ROOT;
	if (_dir_map["client_max_body_size"].empty())
		_dir_map["client_max_body_size"] = DEFAULT_CLIENT_MAX_BODY_SIZE;
	
	// std::cout << "1" << std::endl;
	checkValidNumberValue(*this, "listen");
	// std::cout << "2" << std::endl;
	checkValidNumberValue(*this, "client_max_body_size");
	// std::cout << "3" << std::endl;
	checkValidNumberValue(*this, "keepalive_timeout");
	// std::cout << "4" << std::endl;
	checkValidErrorPage(  _error_page);
	checkAutoIndexValue(  *this);
}

int
	NginxConfig::ServerBlock::findLocationIndexByDir(
		std::string _dir)
{
	int
		_idx = 0;
	if (this->_location.size() != 0)
		for (_idx = 0; _idx < (int)this->_location.size(); _idx++)
		{
			if (this->_location[_idx]._location == _dir)
				return (_idx);
			else if (_dir.substr(_dir.size() - 1, _dir.size()) == "/")
			{
				if (_location[_idx]._location == _dir.substr(0, _dir.size()-1))
					return (_idx);
			}
			else if (_location[_idx]._location.substr(_location[_idx]._location.size() - 1, _location[_idx]._location.size()) == "/")
			{
				if (_location[_idx]._location.substr(0, _location[_idx]._location.size()-1) == _dir)
					return (_idx);
			}
		}
	return (-1);
}
