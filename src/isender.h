#pragma once

namespace knx
{
    template<typename... Args> class ISender
    {
    public:
        virtual void Send(const Args&... args) = 0;

        virtual ~ISender() = default;
    };
}