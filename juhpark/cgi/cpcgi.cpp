#include <iostream>
#include <stdlib.h>

const string ENV[ 24 ] = {
   "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
   "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
   "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
   "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
   "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
   "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
   "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",      
   "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
   "SERVER_SIGNATURE","SERVER_SOFTWARE" };  //환경 변수


int main () 
{
	std::cout << "Content-type:text/html\r\n\r\n";
	std::cout << "<html>\n";
	std::cout << "<head>\n";
	std::cout << "<title>Hello World - First CGI Program</title>\n";
	std::cout << "</head>\n";
	std::cout << "<body>\n";
	std::cout << "<h2>Hello World! This is my first CGI program</h2>\n";
	std::cout << "<table border = \"0\" cellspacing = \"2\">";


	for (int i = 0; i < 24; i++)
	{
		std::cout << "<tr><td>" << ENV[i] << "<\td><td>";

		char *value = getenv(ENV[i].c_str());
		if (value != 0)
			std::cout << value;
		else
			std::cout << "ENV 종범";
		std::cout << "</td></tr>" << std::end;
	}
	std::cout << "</body>\n";
	std::cout << "</html>\n";
   
   return 0;
}
