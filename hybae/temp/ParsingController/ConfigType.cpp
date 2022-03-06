# include				"ConfigBlocks.hpp"

void
	NginxConfig::TypesBlock::setTypeMap(
		std::map<std::string, std::string>&	_type_map,
		std::string&						_type,
		std::string&						_value)
{
	std::vector<std::string>
		_tmp_vec = Parser::getSplitBySpace(_value);
	for (std::size_t _i = 0; _i < _tmp_vec.size(); ++_i)
		_type_map[_tmp_vec[_i]] = _type;
}

void
	NginxConfig::TypesBlock::setTypesBlock(void)
{
	std::string
		_buf = this->_raw_data;
	std::size_t
		_pos = 0;
	
	while (_pos < _buf.size())
	{
		std::string
			_tmp = Parser::getIdentifier(_buf, _pos, "\n", false);
		
		if (Parser::sideSpaceTrim(_tmp).empty())
			continue;
		
		std::size_t
			_tmp_pos = 0;
		std::string
			_tmp_dir = Parser::getIdentifier(_tmp, _tmp_pos, " ", true),
			_tmp_val = Parser::sideSpaceTrim(Parser::getIdentifier(_tmp, _tmp_pos, ";", true));
		setTypeMap(this->_type_map, _tmp_dir, _tmp_val);
	}
}
