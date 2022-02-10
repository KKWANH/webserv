#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP

#include <ctime>
#include <iostrea>

#include "HTTPHandler.hpp"

class HTTPResponseHandler : public HTTPHandler {
  private:
    Phase	_phase;
    FileController::Type	_type;
    std::string				_staticHtml;
    FileController			*_file;
};

#endif
