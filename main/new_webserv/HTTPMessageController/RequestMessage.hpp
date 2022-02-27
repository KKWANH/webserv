#ifndef REQUESTMESSAGE_HPP
# define REQUESTMESSAGE_HPP

class RequestMessage {
	private:
		HTTPData data;
	public:
		RequestMessage(HTTPData _data) : data(_data) {}
};

#endif
