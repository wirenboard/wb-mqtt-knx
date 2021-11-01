#pragma once

#include "knxtelegram.h"
#include <functional>

namespace knx
{
    class IKnxClient
    {
    public:
        virtual void SetOnReceive(const std::function<void(const TTelegram&)>& handler) = 0;
        virtual void Send(const TTelegram& telegram) = 0;

        virtual ~IKnxClient() = default;
    };
}
