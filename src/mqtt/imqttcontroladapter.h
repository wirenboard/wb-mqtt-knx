#pragma once
#include "wblib/any.h"
#include <memory>

namespace mqtt
{
    class IMqttControlAdapter
    {
    public:
        virtual void Send(WBMQTT::TAny&& value) = 0;
        virtual void SetEventHandler(const std::function<void(const WBMQTT::TAny&)>&) = 0;
    };
}
