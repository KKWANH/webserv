#ifndef					PARSER_NGINX_HPP
# define				PARSER_NGINX_HPP

# include				"Parser.hpp"

class					NginxParser : public Parser
{
	public:
		NginxParser(const std::string& _str)
		{

		}

		static void
			skipComment(const std::string& _str, std::size_t& _comment_pos)
		{

		}

		static void
			findBlockSet(const std::string&										_buf,
						 std::stack<int>& 										_str,
						 std::vector<std::pair<std::string, std::size_t> >&		_vec,
						 std::size_t&											_pos)
		{

		}

		static std::string
			getBlockContent(const std::string& _buf, std::size_t& _pos)
		{

		}
};

#endif