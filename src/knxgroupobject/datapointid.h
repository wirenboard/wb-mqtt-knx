#pragma once
#include <stdint.h>
#include <string>

namespace knx
{
    namespace object
    {
        class TDatapointId
        {
        public:
            explicit TDatapointId() = default;
            explicit TDatapointId(uint32_t main);

            TDatapointId(uint32_t main, uint32_t sub);

            uint32_t GetMain() const;
            void SetMain(uint32_t main);
            uint32_t GetSub() const;
            void SetSub(uint32_t sub);
            uint32_t HasSubId() const;

            bool SetFromString(const std::string& stringIndex);
            std::string ToString() const;

        private:
            uint32_t Main{};
            uint32_t Sub{};
            bool HasSub{};
        };
    }
}
