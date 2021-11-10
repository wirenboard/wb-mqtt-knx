#pragma once
#include <memory>
#include "wblib/any.h"

namespace mqtt
{
    class IMqttControlAdapter
    {
    public:
        virtual void Send(WBMQTT::TAny && value) = 0;
        virtual void SetEventHandler(const std::function<void(const WBMQTT::TAny&)>&) = 0;
    };
}
