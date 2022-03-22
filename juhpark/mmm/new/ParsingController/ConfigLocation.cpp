# include				"ConfigBlocks.hpp"

NginxConfig::LocationBlock::LocationBlock(
		std::string		_arg_raw,
		std::string		_arg_loc,
		InheritData		_arg_inh)
	:	NginxBlock(_arg_raw),
		_location(_arg_loc),
		_inherit_data(_arg_inh),
		_is_break(false)
{
	setDirectiveTypes();
	setBlock();
	checkLocationBlock();
	inheritDirectives();
}

void
	NginxConfig::LocationBlock::setDirectiveTypes(
		void)
{
	_dir_case.push_back("root");
	_dir_case.push_back("index");
	_dir_case.push_back("autoindex");
	_dir_case.push_back("error_page");
	_dir_case.push_back("client_max_body_size");
	_dir_case.push_back("return");
	_dir_case.push_back("deny");
	_dir_case.push_back("error_page");
	_dir_case.push_back("cgi_pass");
	_dir_case.push_back("allowed_method");
	_dir_case.push_back("rewrite");
	_dir_case.push_back("break");
	_dir_case.push_back("inner_proxy");
}

void
	NginxConfig::LocationBlock::checkLocationBlock(
		void)
{
	checkValidErrorPage(_error_page);
	checkAutoIndexValue(*this);
	checkValidNumberValue(*this, "client_max_body_size");

	if (_location.empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"Location block must have at least one location");
	if (!_return.empty())
	{
		if (_return.size() != 2)
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"Return directive must have two arguments");
		else if (!Parser::isNumber(_return[0]))
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"Invaild status code in location - return directive");
	}
}

void
	NginxConfig::LocationBlock::inheritDirectives(
		void)
{
	if (_dir_map["root"] == "")
		_dir_map["root"] = _inherit_data._root;
	if (_dir_map["autoindex"].empty())
		_dir_map["autoindex"] = _inherit_data._autoindex;
	if (_dir_map["client_max_body_size"].empty())
		_dir_map["client_max_body_size"] = _inherit_data._client_max_body_size;
	if (_index.empty())
		_index = _inherit_data._index;
	if (_error_page.empty())
		_error_page = _inherit_data._error_page;
}

void
	NginxConfig::LocationBlock::setBlock(
		void)
{
	std::string
		_buf = _raw_data;
	std::size_t
		_pos = 0;

	while (_buf[_pos])
	{
		std::string
			_tmp_line = Parser::getIdentifier(_buf, _pos, "\n", false);
		if (Parser::sideSpaceTrim(_tmp_line).empty())
			continue ;
		
		std::size_t
			_tmp_pos = 0;
		std::string
			_tmp_dir = Parser::sideSpaceTrim(Parser::getIdentifier(_tmp_line, _tmp_pos, " ", true));
		if (find(_dir_case.begin(), _dir_case.end(), _tmp_dir) == _dir_case.end())
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"Invalid directive: " + _tmp_dir + " is not in block: location list");
		else if (!_dir_map[_tmp_dir].empty())
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"Duplicate directive: " + _tmp_dir + " in location context");
		else
		{
			std::string
				_tmp_val = Parser::sideSpaceTrim(Parser::getIdentifier(_tmp_line, _tmp_pos, ";", true));

			if (	 _tmp_dir	==	"index")
				_index			=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	==	"error_page")
				_error_page		=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	==	"allowed_method")
				_allowed_method	=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	==	"inner_proxy")
				_inner_proxy	=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	==	"return")
				_return			=	Parser::getSplitBySpace(_tmp_val);
			else if (_tmp_dir	== 	"break")
				_is_break		= 	true;
			else if (_tmp_dir	==	"rewrite")
				_rewrite		=	Parser::getSplitBySpace(_tmp_val);
			else
			{
				std::vector<std::string>
					_tmp_split = Parser::getSplitBySpace(_tmp_val);
				if (_tmp_split.size() != 1)
					throw ErrorHandler(__FILE__, __func__, __LINE__,	
						"Invalid number/arguments in location[" + _tmp_dir + " directive]");
				_dir_map[_tmp_dir] = _tmp_split[0];
			}
		}
	}
}