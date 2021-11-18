#pragma once

#include "../knxutils.h"
#include "idpt.h"

namespace knx
{
    namespace object
    {
        class TDptRaw: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            bool FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;

            bool FromKnx(const std::vector<uint8_t>& payload) override;

            std::vector<uint8_t> ToKnx() override;

            std::vector<WBMQTT::TAny> ToMqtt() override;

            virtual ~TDptRaw() = default;

        private:
            std::vector<uint8_t> RawData;
        };
    }
}