#pragma once

#include "idpt.h"

namespace knx
{
    namespace object
    {
        class TDpt1: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            std::vector<uint8_t> mqttToKnx(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            std::vector<WBMQTT::TAny> knxToMqtt(const std::vector<uint8_t>& payload) override;

            virtual ~TDpt1() = default;

        private:
            bool B{false};
        };
    }
}
