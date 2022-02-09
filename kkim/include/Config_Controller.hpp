#ifndef					CONFIG_CONTROLLER_HPP
# define				CONFIG_CONTROLLER_HPP

# include				<string>
# include				<map>
# include				<utility>
# include				<iostream>
# include				<fstream>

# include				"Utils.hpp"

# define				DEFAULT_CONFIG		"./config/wsv.config"
# define				DEFAULT_MIME_TYP	"./config/mime.types"

/**
-------------------------------------------------------------
> ConfigController
: add ConfigController and MIMEController

- Protected Variables:
_cnt        			string, Contents
_uri	  	            string, URI
_mim       			  	bool, check is this mimetype or not

- Constructor
bool _is_mim            Set _mim as the bool

- Member functions:
getContent:			    Find config value with key
setUri:			    	Set _uri as arv, save default if the parameter is wrong
                        Only used in non-mime type
setContent:             Read file and save contents
printContent:           Print contents (debugging)
-------------------------------------------------------------
*/

class
	ConfigController
{
	private:
		std::map<std::string, std::string>
			_cnt;
		std::string
			_uri;
		bool
			_mim;
	
	public:
        /**
        Get a bool from user, set _mim as that bool.

        @param is_mim   bool, is this mimetype or not
        */
		ConfigController(bool _is_mim)
		{
			_mim = _is_mim;
			if (_is_mim == false)
				_uri = DEFAULT_CONFIG;
			else
				_uri = DEFAULT_MIME_TYP;
		}

        /**
        Get content's value with parameter key.

        @return         string, value of content(_cnt)
        @param _key     string, _key
        */
		std::string
			getContent(std::string _key)
		{
			if (_mim == false)
				return (_cnt.at(_key));
			else
			{
				if (this->_cnt.find(_key) != _cnt.end())
					return (this->_cnt.find(_key)->second);
				return (this->_cnt.find("bin")->second);
			}
		}

        /**
        Set _uri as parameter: file name.
        If the file's extension is not .config, save original one.

        @param _nam     string, file name
        */
		void
			setUri(std::string _nam)
		{
			if (strcmp(&_nam.c_str()[strlen(_nam.c_str())-7], ".config") == 0)
				_uri = _nam;
			else
				std::cout	<< ANSI_RED << "[ERR] "
							<< ANSI_RES << "Wrong input. default file will be applied." << std::endl
							<< ANSI_BLU << "[INF]" << ANSI_RES << DEFAULT_CONFIG << std::endl;
		}

        /**
        Save configs into content(_cnt).
        Print error if the file is wrong(not exist or not readable).

        @return         bool, is it failed of not
        @param _nam     string, file name
        */
		int
			setContent(std::string _nam)
		{
			if (_mim == false)
				setUri(_nam);

			std::string			_key, _val, _tmp;
			std::ifstream		_fil(_uri.c_str());
			int					_stt, _end;

			// if file couldn't be opened
			if (_fil.is_open() == false) {
				std::cout		<< ANSI_RED << "[ERR] "
								<< ANSI_RES << "File couldn't be opened. Check it!" << std::endl
								<< ANSI_BLU << "[INF] " << ANSI_RES << _uri << std::endl;
				return			(ERROR);
			}

			// if file is empty
			if (_fil.peek() == std::ifstream::traits_type::eof())
			{
				std::cout		<< ANSI_RED << "[ERR] "
								<< ANSI_RES << "File is empty. Check it!" << std::endl
								<< ANSI_BLU << "[INF] " << ANSI_RES << _uri << std::endl;
				return			(ERROR);
			}

			while (getline(_fil, _tmp))
			{
				_stt = 0;
				_end = _tmp.find(": ");
				_key = _tmp.substr(_stt, _end - _stt);
				_stt = _end + 2;
				_end = _tmp.find("\n");
				_val = _tmp.substr(_stt, _end - _stt);
				_cnt.insert(std::pair<std::string, std::string>(_key, _val));
			}
			return				(0);
		}

		void
			printConfig(void)
		{
			std::map<std::string, std::string>::iterator
				_itr;
			_itr = _cnt.begin();
			std::cout		<< ANSI_BLU << "[INF] "
							<< ANSI_RES << "debugging" << std::endl;
			while (true)
			{
				std::cout		<< _itr->first << "\t: " << _itr->second << std::endl;
				++_itr;
				if (_itr == _cnt.end())
					break;
			}
		}
};

#endif