#ifndef WB_MQTT_KNX_EXCEPTIONS_H
#define WB_MQTT_KNX_EXCEPTIONS_H

#include <wblib/exceptions.h>

namespace knx
{
    class TKnxException : public WBMQTT::TBaseException
    {
    public:
        explicit TKnxException() = delete;
        TKnxException(const char* file, int line, const std::string& message);
    };
} // namespace knx

#endif // WB_MQTT_KNX_EXCEPTIONS_H
