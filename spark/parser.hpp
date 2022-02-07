#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ErrorHandler.hpp"

class Parser {
    protected:
        Parser();
        std::string _rawData;
        std::string _fileName;

    public:
        Parser(const std::string& fileName);
        ~Parser();
        const std::string& getRawData() const;
        bool isValidBlockSet (const std::string& buf);
        std::string leftSpaceTrim(std::string s);
        std::string rightSpaceTrim(std::string s);
        static std::string sideSpaceTrim(std::string s);
        static bool isCharInString(const std::string& str, const char c);
        static std::string	getIdentifier(const std::string str, std::size_t& endPos, std::string delimiter, bool checker);
        static std::vector<std::string> getSplitBySpace(std::string str);
        static bool isNumber(const std::string& str);
};

Parser::Parser(const std::string& fileName) : _fileName(fileName) {
    _rawData = "";
    std::ifstream readFile;

    readFile.open(this->_fileName);
    if (!readFile.is_open()){
        throw ErrorHandler("Error: Configuration open error.", ErrorHandler::CRITICAL, "Parser::Parser");
    }
    while (!readFile.eof()) {
        std::string tmp;
        getline(readFile, tmp);
        this->_rawData += tmp;
        this->_rawData += "\n";
    }
    readFile.close();

    if (!isValidBlockSet(this->_rawData)) {
        throw ErrorHandler("Error: bracket pair is not matched.", ErrorHandler::CRITICAL, "Parser::Parser");
    }
}
Parser::~Parser(){};

const std::string& Parser::getRawData() const{
    return _rawData;
}

bool Parser::isValidBlockSet (const std::string& buf) {
    std::size_t pos = 0;

    int leftBracketNum = 0;
    int rightBracketNum = 0;

    while (buf[pos]) {
        if (buf[pos] == '{') {
            leftBracketNum++;
        } else if (buf[pos] == '}') {
            rightBracketNum++;
        }
        ++pos;
    }
    return leftBracketNum == rightBracketNum;
}

std::string Parser::leftSpaceTrim(std::string s) {
    const std::string drop = " ";
    return s.erase(0, s.find_first_not_of(drop));
}

std::string Parser::rightSpaceTrim(std::string s) {
    const std::string drop = " ";
    return s.erase(s.find_last_not_of(drop)+1);
}

std::string Parser::sideSpaceTrim(std::string s) {
    const std::string drop = " ";
    std::string ret = s.erase(s.find_last_not_of(drop)+1);
    ret = ret.erase(0, ret.find_first_not_of(drop));
    return ret;
}

bool Parser::isCharInString(const std::string& str, const char c) {
    std::size_t itr = 0;
    while (str[itr]) {
        if (str[itr] == c)
            return true;
        ++itr;
    }
    return false;
} 

std::string Parser::getIdentifier(const std::string str, std::size_t& endPos, std::string delimiter, bool checker)
{
    size_t wordSize = 0;

    if (checker && str.find(delimiter) == std::string::npos) {
        std::cout << "[DEBUG] String: " << str << std::endl;
        throw ErrorHandler("Error: There is no delimiter[" + delimiter + "].", ErrorHandler::CRITICAL, "Parser::getIdentifier");
    }

    while ((str[endPos] != '\0') && isCharInString(delimiter, str[endPos])) {
        ++endPos;
    }
    size_t begPos = endPos;
    while ((str[endPos] != '\0') && !isCharInString(delimiter, str[endPos])) {
        ++wordSize;
        ++endPos;
    }
    return str.substr(begPos, wordSize);
}

std::vector<std::string> Parser::getSplitBySpace(std::string str) {
    std::vector<std::string> ret;

    std::size_t pos = 0;
    while (pos < str.size()) {
        std::string tmp = getIdentifier(str, pos, " ", false);
        if (tmp.empty()) {
            break ;
        }
        ret.push_back(tmp);
    }
    return ret; 
}

bool Parser::isNumber(const std::string& str) {
    int pos = 0;
    while (str[pos]) {
        if (!(str[pos] >= '0' && str[pos] <= '9')) {
            return false;
        }
        ++pos;
    }
    return true;
}
#endif
