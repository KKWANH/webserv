#include "HTTPData.hpp"

int	HTTPData::getSBlock(void) { return(this->server_block); }
		
HTTPData::HTTPData(int server_block, int server_port, std::string client_ip) {
	this->server_block = server_block;
	this->server_port = server_port;
	this->client_ip = client_ip;
	this->is_autoindex = false;
	this->is_buffer_write = false;
	this->status_code = 200;
	this->is_405 = false;
}
