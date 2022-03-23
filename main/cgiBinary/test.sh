#!/bin/bash

export CONTENT_LENGTH=27
export CONTENT_TYPE="application/x-www-form-urlencoded"
export DOCUMENT_ROOT="/cgiBinary"
export DOCUMENT_URI="/cgiBinary/sample.php"
#export GATEWAY_INTERFACE="CGI/1.1"
export LANG="en_US.UTF-8"
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/munki"
export PATH_INFO="/cgiBinary/php-cgi"
export PWD="/Users/hybae/Desktop/webserv/main/new"
export QUERY_STRING=""
export REMOTE_ADDR="127.0.0.1"
export REQUEST_METHOD="POST"
export REQUEST_SCHEME="POST"
export REQUEST_URI="/cgiBinary/sample.php"
export SCRIPT_FILENAME="/Users/hybae/Desktop/webserv/main/new/cgiBinary/sample.php"
export SCRIPT_NAME="/cgiBinary/php-cgi"
export SERVER_PORT=4242
export SERVER_PROTOCOL="HTTP/1.1"
export SERVER_SOFTWARE="webserv/1.0"
export USER="hybae"


echo -n "param1=value1&param2=value2" | ../cgiBinary/php-cgi "/Users/hybae/Desktop/webserv/main/new/cgiBinary/sample.php" | sleep 10
# ./../cgiBinary/php-cgi ./../cgiBinary/sample.php "123=abc"
