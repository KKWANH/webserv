#!/bin/bash

# CGI/1.1 Standard meta-variable
#export AUTH_TYPE=
export CONTENT_LENGTH=30
export CONTENT_TYPE="php"
export DOCUMENT_ROOT="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary"
export DOCUMENT_URI="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"
export GATEWAY_INTERFACE="CGI/1.1"
export LANG="en_US.UTF-8"
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/share/dotnet:~/.dotnet/tools:/Library/Apple/usr/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands"
export PATH_INFO="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"
export PWD="/Users/hyunja/all_42/00.Table_inworking/webserv/main/new/CGI"
export QUERY_STRING="123=abc"
export REMOTE_ADDR="127.0.0.1"
export REQUEST_METHOD="GET"
export REQUEST_SCHEME="GET"
export REQUEST_URI="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"
export SCRIPT_FILENAME="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"
export SCRIPT_NAME="Users/hyunja/all_42/00.Table_inworking/webserv/main/new/cgiBinary/php-cgi"
export SERVER_PORT=4242
export SERVER_PROTOCOL="HTTP/1.1"
export SERVER_SOFTWARE="webserv/1.0"
export USER="hyunja"

echo "pval=1234&name=joopark" | ../cgiBinary/php-cgi "val=value"