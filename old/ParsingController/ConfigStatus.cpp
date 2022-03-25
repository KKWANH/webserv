# include "ConfigStatus.hpp"

void
	StatusConfig::startConfig(
		std::string _fil_name)
	{
		this->_uri = _fil_name;
		std::string
			_dir,
			_val,
			_tmp;
		int
			_stt,
			_end;
		std::ifstream
			_file_read(_uri.c_str());
		
		if (_file_read.peek() == std::ifstream::traits_type::eof())
			throw ErrorHandler(__FILE__, __func__, __LINE__,
				"status code file is empty..");
		
		while (getline(_file_read, _tmp))
		{
			_stt = 0;
			_end = _tmp.find(": ");
			_dir = _tmp.substr(_stt, _end - _stt);

			_stt = _end + 2;
			_end = _tmp.find("\n");
			_val = _tmp.substr(_stt, _end - _stt);
			_status_map.insert(std::make_pair(_dir.c_str(), _val));
		}
	}
		
std::string
	StatusConfig::getStatus(
		std::string _key)
{
	std::map<std::string, std::string>::iterator
		_itr;
	for (_itr = _status_map.begin(); _itr != _status_map.end(); _itr++)
		if (_itr->first == _key)
			return (_itr->second);
	return (_status_map.find("empty")->second);
}
