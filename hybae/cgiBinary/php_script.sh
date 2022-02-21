# !/bin/bash

# CGI/1.1 Standard meta-variable
#export AUTH_TYPE=
export CONTENT_LENGTH=22
export CONTENT_TYPE="application/x-www-form-urlencoded"
export GATEWAY_INTERFACE="CGI/1.1"
#export PATH_TRANSLATED=
export PATH_INFO="/Users/server/Desktop/BHS/webserv/hybae/cgiBinary"
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
export DOCUMENT_ROOT="/Users/server/Desktop/BHS/webserv/hybae/cgiBinary" # 아님말고
export DOCUMENT_URI="/Users/server/Desktop/BHS/webserv/hybae/cgiBinary/sample.php"
export REQUEST_URI="/Users/server/Desktop/BHS/webserv/hybae/cgiBinary/sample.php"

# it's not in CGI/1.1 standard
export SCRIPT_FILENAME="/Users/server/Desktop/BHS/webserv/hybae/cgiBinary/sample.php"
export REDIRECT_STATUS=200

echo "" | ./php-cgi "val=value"