#ifndef SERVERPROCESS_HPP
# define SERVERPROCESS_HPP

#include "HTTPConnection.hpp"
#include "KernelQueueController.hpp"
#include "SocketController.hpp"
#include "ConfigBlocks.hpp"
#include "TimeController.hpp"
#include <cstring>
#include <fcntl.h>

class ServerProcess {
	public:
		static void		serverProcess();
};

#endif
