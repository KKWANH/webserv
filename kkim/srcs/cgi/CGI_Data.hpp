#ifndef					CGI_DATA_HPP
# define				CGI_DATA_HPP

# include				<iostream>
# include				<map>
# include				<string>

class					CGIData
{
	private:
		std::map<std::string, std::string>
			_env;
		char**
			_arv;
	
	public:
		char*			getFilePath(void)	{ return (_arv[0]); }
		char**			getArgv(void)		{ return (_arv); }
		
		char**
			generateENVP(std::map<std::string, std::string> _env)
		{
			char**
				_envp = new char*[_env.size() + 1];
			int
				_idx = 0;
			std::map<std::string, std::string>::iterator
				_itr;
			
			for (_itr = _env.begin(); _itr != _env.end(); ++_itr)
			{
				_envp[_idx] = new char[(_itr->first.length(), _itr->second.length() + 2)];
				strcpy(_envp[_idx], _itr->first.c_str());
				strcat(_envp[_idx], "=");
				strcat(_envp[_idx], _itr->second.c_str());
				++_idx;
			}
			_envp[_idx] = NULL;
			return		(_envp);
		}

		void
			setCGIArgv(void)
		{
			this->_arv		= new char*[4];
			this->_arv[0]	= new char[22];
			strcpy(_arv[0], "./../../cgiBinary/php-cgi");
			this->_arv[1]	= new char[25];
			strcpy(_arv[1], "./../../cgiBinary/sample.php");
			this->_arv[2]	= new char[10];
			strcpy(_arv[2], "var=1234");
			this->_arv[3]	= NULL;
		}
};

#endif