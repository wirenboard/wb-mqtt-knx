#include "Exception.h"

TKnxException::TKnxException(std::__cxx11::string message) : Message("KNX error: " + message)
{
}

const char* TKnxException::what() const throw()
{
    return Message.c_str();
}

TKnxException::~TKnxException() throw()
{
}
