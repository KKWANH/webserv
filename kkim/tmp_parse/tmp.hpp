#ifndef		CONFIG_HPP
# define	CONFIG_HPP

namespace	NginxConfig
{
	/**
	-> Parser
	 : rawData[string]
	 : fileName[string]
		-> NginxParser
		-> GlobalConfig
		 : none[NoneBlock]
		 : http[HttpBlock]
	
	-> NginxBlock
	 : rawData[string]
	 : dirMap[map_string_string])
		-> NoneBlock
		 : dirCase[vector]
		 : user[string]
		 : worker_process[string]
		-> TypesBlock
		 : typeMap[map_string_string]
		-> LocationBlock
		 : dirCase[vector_string]
		 : locationPath[string]
		 : return[vector_string]
		 : index[vector_string]
		 : error_page[vector_string]
		 : allowed_methods[vector_string]
		 : inner_proxy[vector_string]
		 : inheritData[InheritData]
		-> ServerBlock
		 : dirCase[vector_string]
		 : index[vector_string]
		 : error_page[vector_string]
		 : location[vector_LocationBlock]
		-> HttpBlock
		 : dirCase[vector_string]
		 : server[vector_ServerBlock]
		 : types[TypesBlock]

	-> struct InheritData
	: root[string]
	: autoindex[string]
	: clientMaxBodySize[string]
	: error_page[vector_string]
	: index[vector_string]
	*/
}

#endif