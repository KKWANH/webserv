#ifndef NGINXCONFIG_HPP
#define NGINXCONFIG_HPP

#include "NginxParser.hpp"

namespace NginxConfig{
    class NginxBlock {
        public:
            std::string rawData;
            std::map<std::string, std::string> dirMap;

            NginxBlock() {};
            NginxBlock(std::string rawData) : rawData(rawData) {};

            void checkValidNumberValue(NginxBlock& block, std::string directive);
            void checkValidErrorPage(const std::vector<std::string>& errorPage);
            void checkAutoindexValue(NginxBlock& block);
    };

    class NoneBlock : public NginxBlock {
        public:
            std::vector<std::string> dirCase;
            std::string user;
            std::string worker_processes;
    };

    class TypesBlock : public NginxBlock {
        public:
            std::map<std::string, std::string> typeMap;
            TypesBlock() {}
            TypesBlock(std::string rawData) : NginxBlock(rawData) {
                setTypesBlock();
            }
            void setTypeMap(std::map<std::string, std::string>& typeMap, std::string& type, std::string& value);        
            void setTypesBlock();
    };


typedef struct s_InheritData {
    std::string root;
    std::string autoindex;
    std::string clientMaxBodySize;
    std::vector<std::string> error_page;
    std::vector<std::string> index;
} InheritData;

class LocationBlock : public NginxBlock {
    public:
        std::vector<std::string> dirCase;
        std::string _locationPath;
        std::vector<std::string> _return;
        std::vector<std::string> index;
        std::vector<std::string> error_page;
        std::vector<std::string> allowed_method;
        std::vector<std::string> inner_proxy;
        InheritData _inheritData;

        LocationBlock() {}
        LocationBlock(std::string rawData, std::string locationPath, InheritData inheritData);
        void setDirectiveTypes();
        void checkLocationBlock();
        void inheritDirectives();
        void setBlock();
};

#ifndef DEFAULT_ROOT
#define DEFAULT_ROOT std::string("/usr/share/nginx/html")
#endif
#ifndef DEFAULT_CLIENT_MAX_BODY_SIZE
#define DEFAULT_CLIENT_MAX_BODY_SIZE std::string("100000000")
#endif

class ServerBlock : public NginxBlock{
    public:
        std::vector<std::string> dirCase;
        std::vector<std::string> index;
        std::vector<std::string> error_page;
        std::vector<class LocationBlock> location;

        ServerBlock() {}
        ServerBlock(std::string rawData);
        void setDirectiveTypes();        
        InheritData getInheritData();
        void setBlock();
        void checkServerBlock();

};

class HttpBlock : public NginxBlock{
    public:
        std::vector<std::string> dirCase;
        std::vector<class ServerBlock> server;
        class TypesBlock types;
    
        HttpBlock() {}
        HttpBlock(std::string rawData);
        void setDirectiveTypes();
        void setBlock();
        void checkHttpBlock();
};

class GlobalConfig : public NginxParser {
    public:
        class NoneBlock _none;
        class HttpBlock _http;

        GlobalConfig(const std::string& fileName);
        void checkGlobalConfigBlock();
};


void NginxConfig::NginxBlock::checkValidNumberValue(NginxBlock& block, std::string directive) {
    if (!block.dirMap[directive].empty() && !Parser::isNumber(block.dirMap[directive])) {
        throw ErrorHandler("Error: invalid value in " + directive + " directive.", ErrorHandler::CRITICAL, "NginxBlock::checkValidNumberValue");
    }
}

void NginxConfig::NginxBlock::checkValidErrorPage(const std::vector<std::string>& errorPage) { std::vector<std::string>::const_iterator iter;
    if (!errorPage.empty()) {
        for (iter = errorPage.begin(); iter != errorPage.end() - 1; ++iter) {
            if (!Parser::isNumber(*iter)) {
                throw ErrorHandler("Error: invalid value " + *iter + " in error_page directive.", ErrorHandler::CRITICAL, "NginxBlock::checkValidErrorPage");
            }
        }
    }
}

void NginxConfig::NginxBlock::checkAutoindexValue(NginxBlock& block) {
    if (!block.dirMap["autoindex"].empty() && !(block.dirMap["autoindex"] == "on"
          || dirMap["autoindex"] == "off")) {
        throw std::string("Error: invalid argument for autoindex directive.");
        throw ErrorHandler("Error: invalid argument for autoindex directive.", ErrorHandler::CRITICAL, "NginxBlock::checkAutoindexValue");
    }
}

void NginxConfig::TypesBlock::setTypeMap(std::map<std::string, std::string>& typeMap, std::string& type, std::string& value) {
    std::vector<std::string> tmpVec = Parser::getSplitBySpace(value);
    for (std::size_t i = 0; i < tmpVec.size(); ++i) {
        typeMap[tmpVec[i]] = type;
    }
}

void NginxConfig::TypesBlock::setTypesBlock() {
    std::string buf = rawData;
    std::size_t pos = 0;
    while (pos < buf.size()) {
        std::string tmpLine = Parser::getIdentifier(buf, pos, "\n", false);
        if (Parser::sideSpaceTrim(tmpLine).empty()) {
            continue ;
        }
        std::size_t tmpPos = 0;
        std::string tmpDir = Parser::getIdentifier(tmpLine, tmpPos, " ", true);
        std::string value = Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, ";", true));
        setTypeMap(typeMap, tmpDir, value);
    }
}

NginxConfig::LocationBlock::LocationBlock(std::string rawData, std::string locationPath, InheritData inheritData) : NginxBlock(rawData), _locationPath(locationPath), _inheritData(inheritData) {
    setDirectiveTypes();
    setBlock();
    checkLocationBlock();
    inheritDirectives();
}

void NginxConfig::LocationBlock::setDirectiveTypes() {
    dirCase.push_back("root");
    dirCase.push_back("index");
    dirCase.push_back("autoindex");
    dirCase.push_back("error_page");
    dirCase.push_back("client_max_body_size");

    dirCase.push_back("return");
    dirCase.push_back("try_files");
    dirCase.push_back("deny");
    dirCase.push_back("error_page");
    dirCase.push_back("cgi_pass");
    dirCase.push_back("allowed_method");
    dirCase.push_back("inner_proxy");
}

void NginxConfig::LocationBlock::checkLocationBlock() {
    checkValidErrorPage(error_page);
    checkAutoindexValue(*this);
    checkValidNumberValue(*this, "client_max_body_size");
    if (_locationPath.empty()) {
        throw ErrorHandler("Error: location block doesn't have locationPath.", ErrorHandler::CRITICAL, "LocationBlock::checkLocationBlock");
    }
    if (!_return.empty()) {
        if (_return.size() != 2) {
        throw ErrorHandler("Error: invalid number of arguments in location[return].", ErrorHandler::CRITICAL, "LocationBlock::checkLocationBlock");
        } else if (!Parser::isNumber(_return[0])) {
            throw ErrorHandler("Error: invalid status code in location[return] directive.", ErrorHandler::CRITICAL, "LocationBlock::checkLocationBlock");
        }
    }
}

void NginxConfig::LocationBlock::inheritDirectives() {
    if (dirMap["root"].empty()) {
        dirMap["root"] = _inheritData.root;
    }
    if (index.empty()) {
        index = _inheritData.index;
    }
    if (dirMap["autoindex"].empty()) {
        dirMap["autoindex"] = _inheritData.autoindex;
    }
    if (error_page.empty()) {
        error_page = _inheritData.error_page;
    }
    if (dirMap["client_max_body_size"].empty()) {
        dirMap["client_max_body_size"] = _inheritData.clientMaxBodySize;
    }
}

void NginxConfig::LocationBlock::setBlock() {
    std::string buf = rawData;
    std::size_t pos = 0;
    while (buf[pos]) {
        std::string tmpLine = Parser::getIdentifier(buf, pos, "\n", false);
        if (Parser::sideSpaceTrim(tmpLine).empty()) {
            continue ;
        }
        std::size_t tmpPos = 0;
        std::string tmpDir = Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, " ", true));
        if (find(dirCase.begin(), dirCase.end(), tmpDir) == dirCase.end()) {
            throw ErrorHandler("Error: " + tmpDir + " is not in block[location] list.", ErrorHandler::CRITICAL, "LocationBlock::setBlock");
        } else if (!dirMap[tmpDir].empty()) {
            throw ErrorHandler("Error: There is repeated [" + tmpDir + "] directive. in location context", ErrorHandler::CRITICAL, "LocationBlock::setBlock");
        } else {
            std::string tmpVal = Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, ";", true));
            if (tmpDir == "index") {
                index = Parser::getSplitBySpace(tmpVal);
            } else if (tmpDir == "error_page") {
                error_page = Parser::getSplitBySpace(tmpVal);
            } else if (tmpDir == "allowed_method") {
                allowed_method = Parser::getSplitBySpace(tmpVal);
            } else if (tmpDir == "inner_proxy") {
                inner_proxy = Parser::getSplitBySpace(tmpVal);
            } else if (tmpDir == "return") {
                _return = Parser::getSplitBySpace(tmpVal);
            } else {
                std::vector<std::string> tmpSplit = Parser::getSplitBySpace(tmpVal);
                if (tmpSplit.size() != 1) {
                    throw ErrorHandler("Error: invalid number of arguments in location["+ tmpDir + " directive].", ErrorHandler::CRITICAL, "LocationBlock::setBlock");
                }
                dirMap[tmpDir] = tmpSplit[0];
            }
        }
    }
}

NginxConfig::ServerBlock::ServerBlock(std::string rawData) : NginxBlock(rawData) {
    setDirectiveTypes();
    setBlock();
    checkServerBlock();
}

void NginxConfig::ServerBlock::setDirectiveTypes() {
    dirCase.push_back("root");
    dirCase.push_back("index");
    dirCase.push_back("autoindex");
    dirCase.push_back("error_page");

    dirCase.push_back("listen");
    dirCase.push_back("server_name");
    dirCase.push_back("location");
    dirCase.push_back("client_max_body_size");
    dirCase.push_back("keepalive_timeout");
}

NginxConfig::InheritData NginxConfig::ServerBlock::getInheritData() {
    InheritData inheritData;
    if (!dirMap["root"].empty()) {
        inheritData.root = dirMap["root"];
    }
    if (!dirMap["autoindex"].empty()) {
        inheritData.autoindex = dirMap["autoindex"];
    }
    if (!index.empty()) {
        inheritData.index = index;
    }
    if (!error_page.empty()) {
        inheritData.error_page = error_page;
    }
    if (!dirMap["client_max_body_size"].empty()) {
        inheritData.clientMaxBodySize = dirMap["client_max_body_size"];
    }
    return inheritData;
}

void NginxConfig::ServerBlock::setBlock() {
    std::string buf = rawData;
    std::size_t pos = 0;
    std::size_t blockPos = 0;
    while (buf[pos]) {
        std::string tmpLine = Parser::getIdentifier(buf, pos, "\n", false);
        if (Parser::sideSpaceTrim(tmpLine).empty()) {
            continue ;
        }
        std::size_t tmpPos = 0;
        std::string tmpDir = Parser::getIdentifier(tmpLine, tmpPos, " ", true);
        if (find(dirCase.begin(), dirCase.end(), tmpDir) == dirCase.end()) {
            throw ErrorHandler("Error: " + tmpDir + " is not in context[server] list.", ErrorHandler::CRITICAL, "ServerBlock::setBlock");
        } else if (tmpDir == "location") {
            InheritData inheritData = getInheritData();
            LocationBlock tmpLocationBlock(NginxParser::getBlockContent(buf, blockPos), Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, "{", true)), inheritData);
            location.push_back(tmpLocationBlock);
            pos = blockPos;
        } else if (!dirMap[tmpDir].empty()) {
            throw ErrorHandler("Error: There is repeated [" + tmpDir + "] directive in server context", ErrorHandler::CRITICAL, "ServerBlock::setBlock");
        } else {
            std::string tmpVal = Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, ";", true));
            if (tmpDir == "index") {
                index = Parser::getSplitBySpace(tmpVal);
            } else if (tmpDir == "error_page") {
                error_page = Parser::getSplitBySpace(tmpVal);
            } else {
                std::vector<std::string> tmpSplit = Parser::getSplitBySpace(tmpVal);
                if (tmpSplit.size() != 1) {
                    throw ErrorHandler("Error: invalid number of arguments in server["+ tmpDir + " directive].", ErrorHandler::CRITICAL, "ServerBlock::setBlock");
                }
                dirMap[tmpDir] = tmpSplit[0];
            }
        }
    }
}

void NginxConfig::ServerBlock::checkServerBlock() {
    if (location.empty()) {
        throw ErrorHandler("Error: There is no location context in server context.", ErrorHandler::CRITICAL, "ServerBlock::checkServerBlock");
    }
    if (dirMap["listen"].empty()) {
        throw ErrorHandler("Error: There is no port number for listening.", ErrorHandler::CRITICAL, "ServerBlock::checkServerBlock");
    }
    if (dirMap["root"].empty()) {
        dirMap["root"] = DEFAULT_ROOT;
    }
    if (dirMap["client_max_body_size"].empty()) {
        dirMap["root"] = DEFAULT_CLIENT_MAX_BODY_SIZE;
    }
    checkValidNumberValue(*this, "listen");
    checkValidNumberValue(*this, "client_max_body_size");
    checkValidNumberValue(*this, "keepalive_timeout");
    checkValidErrorPage(error_page);
    checkAutoindexValue(*this);
}

NginxConfig::HttpBlock::HttpBlock(std::string rawData) : NginxBlock(rawData) {
    setDirectiveTypes();
    setBlock();
    checkHttpBlock();
}

void NginxConfig::HttpBlock::setDirectiveTypes() {
    dirCase.push_back("charset");
    dirCase.push_back("default_type");
    dirCase.push_back("keepalive_timeout");
    dirCase.push_back("sendfile");
    dirCase.push_back("types");
    dirCase.push_back("server");
}

void NginxConfig::HttpBlock::setBlock() {
    std::size_t pos = 0;
    std::size_t blockPos = 0;
    bool typesOnlyOneFlag = true;
    while (rawData[pos]) {
        std::string tmpLine = Parser::getIdentifier(rawData, pos, "\n", false);
        if (Parser::sideSpaceTrim(tmpLine).empty()) {
            continue ;
        }
        std::size_t tmpPos = 0;
        std::string tmpDir = Parser::getIdentifier(tmpLine, tmpPos, " ", true);
        if (find(dirCase.begin(), dirCase.end(), tmpDir) == dirCase.end()) {
            throw ErrorHandler("Error: " + tmpDir + " is not in context[http] list.", ErrorHandler::CRITICAL, "HttpBlock::setBlock");
        } else if (tmpDir == "server") {
            ServerBlock tmpServerBlock(NginxParser::getBlockContent(rawData, blockPos));
            server.push_back(tmpServerBlock);
            pos = blockPos;
        } else if (!dirMap[tmpDir].empty()) {
            throw ErrorHandler("Error: There is repeated [" + tmpDir + "] directive. in http context", ErrorHandler::CRITICAL, "HttpBlock::setBlock");
        } else if (tmpDir == "types") {
            if (!typesOnlyOneFlag) {
                throw ErrorHandler("Error: types context must exist only one!", ErrorHandler::CRITICAL, "HttpBlock::setBlock");
            }
            typesOnlyOneFlag = false;
            TypesBlock tmpTypesBlock(NginxParser::getBlockContent(rawData, blockPos));
            types = tmpTypesBlock;
            pos = blockPos;
        } else {
            std::string tmpVal = Parser::sideSpaceTrim(Parser::getIdentifier(tmpLine, tmpPos, ";", true));
            std::vector<std::string> tmpSplit = Parser::getSplitBySpace(tmpVal);
            if (tmpSplit.size() != 1) {
                throw ErrorHandler("Error: invalid number of arguments in http["+ tmpDir + " directive].", ErrorHandler::CRITICAL, "HttpBlock::setBlock");
            }
            dirMap[tmpDir] = tmpSplit[0];
        }
    }
}

void NginxConfig::HttpBlock::checkHttpBlock() {
    std::vector<ServerBlock>::iterator iter;
    std::vector<std::string> listens;
    for (iter = server.begin(); iter != server.end(); ++iter) {
        listens.push_back(iter->dirMap["listen"]);
    }
    if (!(std::unique(listens.begin(), listens.end()) == listens.end())) {
        throw ErrorHandler("Error: There is repeated listening port in different server context", ErrorHandler::CRITICAL, "HttpBlock::checkHttpBlock");
    }
}

NginxConfig::GlobalConfig::GlobalConfig(const std::string& fileName) : NginxParser(fileName) {
    std::size_t pos = 0;
    std::string identifier;

    bool httpOnlyOneFlag = true;
    while (_rawData[pos]) {
        std::string tmpLine = getIdentifier(_rawData, pos, "\n", false);
        if (sideSpaceTrim(tmpLine).empty()) {
            continue ;
        }

        std::size_t tmpPos = 0;
        std::string tmpDir = getIdentifier(tmpLine, tmpPos, " ", true);
        std::size_t blockPos = 0;
        if (tmpDir == "http") {
            if (!httpOnlyOneFlag) {
                throw ErrorHandler("Error: http context must exist only one!", ErrorHandler::CRITICAL, "NginxConfig::GlobalConfig");
            }
            httpOnlyOneFlag = false;
            HttpBlock tmpHttpBlock(NginxParser::getBlockContent(_rawData, blockPos));
            _http = tmpHttpBlock;
            pos = blockPos;
        } else {
            std::string tmpVal = getIdentifier(tmpLine, tmpPos, ";", true);
            if (tmpDir == "user") {
                _none.user = sideSpaceTrim(tmpVal);
            } else if (tmpDir == "worker_processes") {
                _none.worker_processes = sideSpaceTrim(tmpVal);
            } else {
                throw ErrorHandler("Error: " + tmpDir + " is not in block[none] list.", ErrorHandler::CRITICAL, "NginxConfig::GlobalConfig");
            }
        }
    }
    checkGlobalConfigBlock();
}

void NginxConfig::GlobalConfig::checkGlobalConfigBlock() {
    if (_http.server.empty()) {
        throw ErrorHandler("Error: There is no server block in http block.", ErrorHandler::CRITICAL, "NginxConfig::GlobalConfig");
    } else if (_http.dirMap["default_type"].empty()) {
        throw ErrorHandler("Error: There is no default_type[directive] in configuration file.", ErrorHandler::CRITICAL, "NginxConfig::GlobalConfig");
    }

}

#endif
