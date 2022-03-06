#ifndef					PARSER_HPP
# define				PARSER_HPP

// # include				<string>
# include				<map>
# include				<vector>
# include				<utility>
# include				<iostream>
# include				<fstream>

# include				"../ErrorHandler/ErrorHandler.hpp"

class					Parser
{
	protected:
		Parser(void);
		std::string
			_raw_data;
		std::string
			_file_name;
	
	public:
		// 파일 name
		Parser(					const std::string&		_str);
		~Parser(void);
		
		const std::string&
			getRawData(void) const;
		bool
			isValidBlockSet(	const std::string&		_buf);
		std::string
			leftSpaceTrim(		std::string				_str);
		std::string
			rightSpaceTrim(		std::string				_str);
		static std::string
			sideSpaceTrim(		std::string				_str);
		static bool
			isCharInString(		const std::string&		_str,
								const char				_chr);
		static std::string
			getIdentifier(		const std::string		_str,
								std::size_t&			_end_pos,
								std::string				_delimiter,
								bool					_checker);
		static std::vector<std::string>
			getSplitBySpace(	std::string				_str);
		static bool
			isNumber(			const std::string&		_str);
};

#endif