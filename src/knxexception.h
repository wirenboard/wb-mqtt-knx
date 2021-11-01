#pragma once

#include <wblib/exceptions.h>

namespace knx
{
    class TKnxException: public WBMQTT::TBaseException
    {
    public:
        explicit TKnxException() = delete;
        TKnxException(const char* file, int line, const std::string& message);
    };
}
