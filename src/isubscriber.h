#pragma once

namespace knx
{
    template<typename T> class ISubscriber
    {
    public:
        virtual void Notify(const T& data) = 0;
    };
}