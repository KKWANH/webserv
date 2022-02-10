#ifndef NGINXPARSER_HPP
#define NGINXPARSER_HPP

#include "Parser.hpp"

class NginxParser : public Parser {
    public:        
        NginxParser(const std::string& fileName);
        static void skipComment(const std::string& str, std::size_t &commentPos);
        static void findBlockSet(const std::string& buf, std::stack<int>st, std::vector<std::pair<std::string, std::size_t> >& vec, std::size_t& pos);
        static std::string getBlockContent(const std::string& buf, std::size_t& pos);
};


NginxParser::NginxParser(const std::string& fileName) : Parser(fileName) {
}

void NginxParser::skipComment(const std::string& str, std::size_t &commentPos) {
    while (str[commentPos] == ' ') {
        ++commentPos;
    }
    if (!(str[commentPos] == '#')) {
        return ;
    }
    while (str[commentPos]) {
        if (str[commentPos] != '\n') {
            ++commentPos;
        } else {
            return ;
        }
    }
}

void NginxParser::findBlockSet(const std::string& buf, std::stack<int>st, std::vector<std::pair<std::string, std::size_t> >& vec, std::size_t& pos) {
    while (!(buf[pos] == '\0' || buf[pos] == '{' || buf[pos] == '}')) {
        ++pos;
    }
    if (buf[pos] == '{') {
        st.push(1);
        vec.push_back(std::make_pair("{", pos));
    } else if (buf[pos] == '}') {
        if (st.top() == 1) {
            st.pop();
        }
        vec.push_back(std::make_pair("}", pos));
    }
    if (st.empty()) {
        return ;
    }
    pos += 1;
    findBlockSet(buf, st, vec, pos);
}

std::string NginxParser::getBlockContent(const std::string& buf, std::size_t& pos) {
    std::vector<std::pair<std::string, std::size_t> > blockSet;
    std::stack<int> st;

    findBlockSet(buf, st, blockSet, pos);
    if (blockSet.empty()) {
        throw std::string("Error: bracket is empty. NginxParser::getBlockContent");
        throw ErrorHandler("Error: bracket is empty. NginxParser::getBlockContent", ErrorHandler::CRITICAL, "NginxParser::getBlockContent");
    }
    std::size_t blockBeg = blockSet.begin()->second;
    std::size_t blockEnd = (blockSet.end() - 1)->second;
    pos = blockEnd + 1;

    return buf.substr(blockBeg + 1, blockEnd - blockBeg - 1);
}




#endif