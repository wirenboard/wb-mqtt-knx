#pragma once

namespace knx
{
    template<typename... Args> class ISubscriber
    {
    public:
        virtual void Notify(const Args&... args) = 0;
    };
}