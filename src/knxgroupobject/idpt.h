#pragma once

#include "wblib/any.h"

namespace knx
{
    namespace object
    {
        struct DptDescriptorField
        {
            std::string Name;
            std::string Type;
            double Min;
            double Max;
        };

        class IDpt
        {
        public:
            virtual std::vector<DptDescriptorField> getDescriptor() const = 0;

            virtual bool FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) = 0;
            virtual bool FromKnx(const std::vector<uint8_t>& payload) = 0;

            virtual std::vector<uint8_t> ToKnx() = 0;
            virtual std::vector<WBMQTT::TAny> ToMqtt() = 0;
        };
    }
}