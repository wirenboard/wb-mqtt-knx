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
            double min;
            double max;
        };

        class IDpt
        {
        public:
            virtual std::vector<DptDescriptorField> getDescriptor() const = 0;
            virtual std::vector<uint8_t> mqttToKnx(uint32_t controlIndex, const WBMQTT::TAny& value) = 0;
            virtual std::vector<WBMQTT::TAny> knxToMqtt(const std::vector<uint8_t>& payload) = 0;
        };
    }
}