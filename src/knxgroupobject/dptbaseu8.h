#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Base Datapoint Types "8-Bit Unsigned Value"
        ///
        /// - ID:
        /// - Format: 8 bit: U8
        /// - Field names: UnsignedValue
        class TDptBaseU8: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            uint8_t FieldUnsignedValue = 0;
        };
    }
}
