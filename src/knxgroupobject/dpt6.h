#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        class TDpt6: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            bool FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            bool FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            int8_t FieldRelSignedValue = 0;
        };
    }
}
