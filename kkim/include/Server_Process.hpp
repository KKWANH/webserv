#ifndef					SERVER_PROCESS_HPP
# define				SERVER_PROCESS_HPP

# include				"./HTTPMsg_Controller.hpp"
# include				"./Socket_Controller.hpp"
# include				"./KQueue_Controller.hpp"
# include				"./Config_Controller.hpp"
# include				<dirent.h>
# include				<sys/stat.h>

extern ConfigController	_config;

class
	ServerProcess
{
	public:
		static int
			serverProcess(SocketController* _sck, KQueueController* _kqu)
		{
			while (true)
			{
				_kqu->setPollingCount(
					kevent(
						_kqu->getKq(),
						_kqu->getChangeList(),
						_kqu->getChangeCount(),
						_kqu->getEventList(),
						BUFSIZ, NULL));
				_kqu->clearChangeList();
				_kqu->resetChangeCount();

				for (int _idx = 0; _idx < _kqu->getPollingCount(); _idx++)
				{
					if (_kqu->getEventList(_idx)->filter == EVFILT_READ)
					{
						std::cout << "[READ]\t";
						if (_kqu->getEventList(_idx)->ident == _sck->getSocketServer())
						{
							_sck->setSocketClient(accept(_sck->getSocketServer(), _sck->getConvertedAddressClient(), _sck->getSocketLength()));
							fcntl(_sck->getSocketClient(), F_SETFL, O_NONBLOCK);
							_kqu->setReadKqueue(_sck->getSocketClient());
							std::cout << "Server connect : [" << _sck->getSocketClient() << "]" << std::endl;
						}
						else
						{
							int
								_fd = _kqu->getEventList(_idx)->ident;
							char
								_buf[BUFSIZ];
							recv(_fd, _buf, BUFSIZ, 0);
							if (_kqu->addRequestMessage(_fd, _buf) == -1)
								return (ERROR);
							_kqu->setWriteKqueue(_fd);
						}
					}
					else if (_kqu->getEventList(_idx)->filter == EVFILT_WRITE)
					{
						int
							_fd = _kqu->getEventList(_idx)->ident;
						std::cout << "[WRITE]\tmessage : [" << _fd << "]" << std::endl;
						_kqu->getRequestMessage(_fd)->printRequestMessage();
						std::string
							_msg = ResponseMessage::setResponseMessage(_kqu->getRequestMessage(_fd));
						write(_fd, _msg.c_str(), _msg.size());
						_kqu->removeRequestMessage(_fd);
						close(_fd);
					}
				}
			}
		}
};

#endif