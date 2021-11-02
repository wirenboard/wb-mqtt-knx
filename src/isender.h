#pragma once

namespace knx{
    template<typename T>
    class ISender{
    public:
        virtual void Send(const T& data) = 0;

        virtual ~ISender() = default;
    };
}