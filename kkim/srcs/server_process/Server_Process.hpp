#ifndef					SERVER_PROCESS_HPP
# define				SERVER_PROCESS_HPP

# include				"../http_message/HTTPMsg_Controller.hpp"
# include				"../socket/Socket_Controller.hpp"
# include				"../kqueue/KQueue_Controller.hpp"
# include				"../parse/Config_Controller.hpp"
# include				<dirent.h>
# include				<sys/stat.h>

# define				TMP_BUF_SIZ 20480

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
				// set Event on queue.
				_kqu->setPollingCount(
					kevent(
						_kqu->getKq(),
						_kqu->getChangeList(),
						_kqu->getChangeCount(),
						_kqu->getEventList(),
						BUFSIZ, NULL));
				
				// clear events
				_kqu->clearChangeList();
				_kqu->resetChangeCount();

				// roop for events those are remaining on queue.
				for (int _idx = 0; _idx < _kqu->getPollingCount(); _idx++)
				{
					if (_kqu->getEventList(_idx)->filter == EVFILT_READ)
					{
						// server read
						std::cout << "[READ]\t";
						if ((int)_kqu->getEventList(_idx)->ident == _sck->getSocketServer())
						{
							_sck->setSocketClient(accept(_sck->getSocketServer(), _sck->getConvertedAddressClient(), _sck->getSocketLength()));
							fcntl(_sck->getSocketClient(), F_SETFL, O_NONBLOCK);
							_kqu->setReadKqueue(_sck->getSocketClient());
							std::cout << "Server connect : [" << _sck->getSocketClient() << "]" << std::endl;
						}
						// TODO: file이 너무 클 경우 처리
						// client read
						else
						{
							int
								_fd = _kqu->getEventList(_idx)->ident;
							char
								_buf[TMP_BUF_SIZ];
							int
								_num;

							_num = read(_fd, _buf, TMP_BUF_SIZ - 1);
							std::cout << ANSI_BLU << "[_num]" << ANSI_RES << " : " << _num << std::endl;

							if (_num == ERROR)
							{
								std::cout	<< ANSI_RED << "[ERR] "
											<< ANSI_RES << "recv error" << std::endl;
								return (ERROR);
							}
							else if (_num == TMP_BUF_SIZ - 1)
							{
								_buf[_num] = '\0';
								_kqu->sumMessage(_fd, _buf);
							}
							else
							{
								_buf[_num] = '\0';
								_kqu->sumMessage(_fd, _buf);

								std::cout	<< ANSI_BLU << "[INF]"
											<< ANSI_RES << "Get all" << std::endl;
								if (_kqu->addRequestMessage(_fd) == ERROR)
									return (ERROR);
								_kqu->setWriteKqueue(_fd);

								std::string
									_tmp = ResponseMessage::setResponseMessage(
										_kqu->getRequestMessage(_fd));
								_kqu->saveResponseMessage(_fd, _tmp);
							}
						}
					}
					// write 
					else if (_kqu->getEventList(_idx)->filter == EVFILT_WRITE)
					{
						int
							_fd = _kqu->getEventList(_idx)->ident;
						std::cout	<< ANSI_BLU << "[WRITE]\t"
									<< ANSI_RES << "message : [" << _fd << "]" << std::endl;
						if (_kqu->writeResponseMessage(_fd, TMP_BUF_SIZ) != TMP_BUF_SIZ)
						{
							std::cout	<< ANSI_GRE << "[SUC] "
										<< ANSI_RES << "write success" << std::endl;
							_kqu->removeRequestMessage(_fd);
							close(_fd);
						}
					}
				}
			}
		}
};

#endif