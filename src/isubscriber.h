#pragma once
#include <memory>

namespace knx
{
    template<typename... Args> class ISubscriber
    {
    public:
        virtual void Notify(const Args&... args) = 0;

        virtual ~ISubscriber() = default;
    };

    template<typename... Args> using PSubscriber = std::shared_ptr<ISubscriber<Args...>>;
}