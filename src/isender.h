#pragma once

namespace knx
{
    template<typename... Args> class ISender
    {
    public:
        virtual void Send(const Args&... args) const = 0;

        virtual ~ISender() = default;
    };
}