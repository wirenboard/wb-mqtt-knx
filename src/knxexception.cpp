#include "knxexception.h"

using namespace knx;

TKnxException::TKnxException(const char* file, int line, const std::string& message)
    : WBMQTT::TBaseException(file, line, message)
{}