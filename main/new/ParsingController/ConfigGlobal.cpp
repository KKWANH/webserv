# include				"ConfigBlocks.hpp"

NginxConfig::GlobalConfig::GlobalConfig(
		const std::string& _str)
	:	NginxParser(_str)
{
	std::size_t
		_pos = 0;
	std::string
		_idf;
	bool
		_http_only_one_flag = true;
	
	while (_raw_data[_pos])
	{
		std::string
			_tmp_line = getIdentifier(_raw_data, _pos, "\n", false);
		if (sideSpaceTrim(_tmp_line).empty())
			continue;
		
		std::size_t
			_tmp_pos = 0,
			_block_pos = 0;
		std::string
			_tmp_dir = getIdentifier(_tmp_line, _tmp_pos, " ", true);
		if (_tmp_dir == "http")
		{
			if (!_http_only_one_flag)
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"http context must exist only one: " + _tmp_dir);
			_http_only_one_flag = false;
			HttpBlock
				_tmp_http_block(getBlockContent(_raw_data, _block_pos));
			_http = _tmp_http_block;
			_pos = _block_pos;
		}
		else
		{
			std::string
				_tmp_val = getIdentifier(_tmp_line, _tmp_pos, ";", true);
			if (	 _tmp_dir		== "user")
				_none._user			=  sideSpaceTrim(_tmp_val);
			else if (_tmp_dir		== "worker_processes")
				_none._worker_prc	=  sideSpaceTrim(_tmp_val);
			else
				throw ErrorHandler(__FILE__, __func__, __LINE__,
					"Unknown directive: " + _tmp_dir);
		}
	}
	checkGlobalConfigBlock();
}

void
	NginxConfig::GlobalConfig::checkGlobalConfigBlock(
		void)
{
	if (_http._server.empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"http context must have server block");
	else if (_http._dir_map["default_type"].empty())
		throw ErrorHandler(__FILE__, __func__, __LINE__,
			"http context must have default_type directive");
}