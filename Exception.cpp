#include "Exception.h"

TKnxException::TKnxException(std::string message) : Message("KNX error: " + message)
{
}

const char* TKnxException::what() const throw()
{
    return Message.c_str();
}

TKnxException::~TKnxException() throw()
{
}
