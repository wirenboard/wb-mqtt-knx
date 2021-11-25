#pragma once
#include <memory>

namespace knx
{
    template<typename... Args> class ISender
    {
    public:
        virtual void Send(const Args&... args) = 0;

        virtual ~ISender() = default;
    };

    template<typename... Args> using PSender = std::shared_ptr<ISender<Args...>>;
}