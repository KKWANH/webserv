#ifndef					CONFIGBLOCKS_HPP
# define				CONFIGBLOCKS_HPP

# include				"NginxParser.hpp"

namespace				NginxConfig
{
	typedef struct		s_InheritData
	{
		std::string	
			_root;
		std::string
			_autoindex;
		std::string
			_client_max_body_size;
		std::vector<std::string>
			_error_page;
		std::vector<std::string>
			_index;
	}					InheritData;

	#define				DEFAULT_ROOT					std::string("/usr/share/nginx/html")
	#define				DEFAULT_CLIENT_MAX_BODY_SIZE	std::string("100000000")

	class				NginxBlock
	{
		public:
			std::string
				_raw_data;
			std::map<std::string, std::string>
				_dir_map;
			
			NginxBlock(void) {}
			NginxBlock(std::string _str)
			{
				this->_raw_data = _str;
			}

			void
				checkValidNumberValue(NginxBlock& _block, std::string _dir);
			void
				checkValidErrorPage(const std::vector<std::string>& _err_page);
			void
				checkAutoIndexValue(NginxBlock& _block);
	};

	class				NoneBlock		: public NginxBlock
	{
		public:
			std::vector<std::string>
				_dir_case;
			std::string
				_user;
			std::string
				_worker_prc;
	};

	class				TypesBlock		: public NginxBlock
	{
		public:
			std::map<std::string, std::string>
				_type_map;
			
			TypesBlock(void) {}
			TypesBlock(std::string _str) : NginxBlock(_str) { this->setTypesBlock(); }

			void
				setTypeMap( std::map<std::string, std::string>&	_type_map,
							std::string&						_type,
							std::string&						_value);
			void
				setTypesBlock(void);
	};

	class 				LocationBlock	: public NginxBlock
	{
		public:
			std::vector<std::string>
				_dir_case;
			std::string
				_location;
			std::vector<std::string>
				_return;
			std::vector<std::string>
				_index;
			std::vector<std::string> 
				_error_page;
			std::vector<std::string> 
				_allowed_method;
			std::vector<std::string> 
				_inner_proxy;
			InheritData 
				_inherit_data;

			LocationBlock() {}
			LocationBlock(	std::string _arg_raw,
							std::string _arg_loc,
							InheritData	_arg_inh);

			void
				setDirectiveTypes(void);
			void
				checkLocationBlock(void);
			void
				inheritDirectives(void);
			void
				setBlock(void);
	};

	class				ServerBlock		: public NginxBlock
	{
		public:
        	std::vector<std::string>
				_dir_case;
			std::vector<std::string>
				_index;
			std::vector<std::string>
				_error_page;
			std::vector<class LocationBlock>
				_location;

			ServerBlock(void) {}
			ServerBlock(std::string _str);
			void
				setDirectiveTypes(void);
			InheritData
				getInheritData(void);
			void
				setBlock(void);
			void
				checkServerBlock(void);
	};

	class				HttpBlock		: public NginxBlock
	{
		public:
			std::vector<std::string>
				_dir_case;
			std::vector<class ServerBlock>
				_server;
			class TypesBlock
				_types;
		
			HttpBlock() {}
			HttpBlock(std::string _str);
			void
				setDirectiveTypes(void);;
			void
				setBlock(void);;
			void
				checkHttpBlock(void);
	};

	class				GlobalConfig	: public NginxParser
	{
		public:
			class NoneBlock
				_none;
			class HttpBlock
				_http;
			
			GlobalConfig(
					const std::string& _str);
			// void
			// 	startConfig(
			// 		const std::string& _str);
			void
				checkGlobalConfigBlock(
					void);
	};
};

#endif