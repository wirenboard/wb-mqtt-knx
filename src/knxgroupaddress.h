#pragma once

#include "eibclient.h"
#include "knxexception.h"
#include "wblib/exceptions.h"
#include <cstdint>
#include <sstream>
#include <wblib/utils.h>

namespace knx
{
    class TKnxGroupAddress
    {
    public:
        explicit TKnxGroupAddress() = default;

        TKnxGroupAddress(uint32_t main, uint32_t middle, uint32_t sub);
        TKnxGroupAddress(uint32_t main, uint32_t sub);

        explicit TKnxGroupAddress(const std::string& str);

        explicit TKnxGroupAddress(eibaddr_t eibAddress);

        uint32_t GetMainGroup() const;

        uint32_t GetMiddleGroup() const;

        uint32_t GetSubGroup() const;

        eibaddr_t GetEibAddress() const;

        bool operator<(const TKnxGroupAddress& rhs) const;

        bool operator==(const TKnxGroupAddress& rhs) const;

        std::string ToString() const;

    private:
        void Init(uint32_t main, uint32_t middle, uint32_t sub);

        void Init(uint32_t main, uint32_t sub);

        uint32_t MainGroup{0};
        uint32_t MiddleGroup{0};
        uint32_t SubGroup{0};
    };
}
