# !/bin/bash

# CGI/1.1 Standard meta-variable
#export AUTH_TYPE=
export CONTENT_LENGTH=22
export CONTENT_TYPE="application/x-www-form-urlencoded"
export GATEWAY_INTERFACE="CGI/1.1"
#export PATH_TRANSLATED=
export PATH_INFO="/Users/sunghyunpark/Desktop/webserv/main/test"
export QUERY_STRING="val=1234"
#export REMOTE_ADDR=127.0.0.1
#export REMOTE_HOST=127.0.0.1
#export REMOTE_IDENT=
#export REMOTE_USER=
export REQUEST_METHOD=POST
export SCRIPT_NAME="sample.php"
#export SERVER_NAME=
#export SERVER_PORT=
export SERVER_SOFTWARE=WEBSERV/0.1
export SERVER_PROTOCOL=HTTP/1.1
export DOCUMENT_ROOT="/Users/sunghyunpark/Desktop/webserv/main/test" # 아님말고
export DOCUMENT_URI="/Users/sunghyunpark/Desktop/webserv/main/test/sample.php"
export REQUEST_URI="/Users/sunghyunpark/Desktop/webserv/main/test/sample.php"

# it's not in CGI/1.1 standard
export SCRIPT_FILENAME="/Users/sunghyunpark/Desktop/webserv/main/test/sample.php"
export REDIRECT_STATUS=200

echo "" | ./php-cgi "val=value"