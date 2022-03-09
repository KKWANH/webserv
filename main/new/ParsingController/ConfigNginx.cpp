# include				"ConfigBlocks.hpp"

void
	NginxConfig::NginxBlock::checkValidNumberValue(
		NginxBlock&		_block,
		std::string		_dir)
{
	if (!_block._dir_map[_dir].empty() &&
		!Parser::isNumber(_block._dir_map[_dir]))
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"Invalid number value for directive: " + _dir);
}

void
	NginxConfig::NginxBlock::checkValidErrorPage(const std::vector<std::string>& _err_page)
{
	std::vector<std::string>::const_iterator
		_itr;
	if (!_err_page.empty())
		for (_itr = _err_page.begin(); _itr != _err_page.end() - 1; ++_itr)
			if (!Parser::isNumber(*_itr))
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"Invalid number value for error page: " + *_itr);
}

void
	NginxConfig::NginxBlock::checkAutoIndexValue(NginxBlock& _block)
{
	if (!_block._dir_map["autoindex"].empty() &&
		!(_block._dir_map["autoindex"] == "on" ||
		  _block._dir_map["autoindex"] == "off"))
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"Invalid number value for directive: autoindex: " + _block._dir_map["autoindex"]);
}
