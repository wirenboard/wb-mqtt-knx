#pragma once

#include "eibclient.h"
#include "knxexception.h"
#include "wblib/exceptions.h"
#include <cstdint>

namespace knx
{
    class TKnxGroupAddress
    {
    public:
        explicit TKnxGroupAddress() = default;

        TKnxGroupAddress(uint32_t main, uint32_t middle, uint32_t sub);

        explicit TKnxGroupAddress(eibaddr_t eibAddress);

        eibaddr_t GetEibAddress() const;

        bool operator<(const TKnxGroupAddress& rhs) const;

    private:
        uint32_t MainGroup{0};
        uint32_t MiddleGroup{0};
        uint32_t SubGroup{0};
    };
}
