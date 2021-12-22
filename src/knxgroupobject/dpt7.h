#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "2-Octet Unsigned Value"
        ///
        /// - ID: 7.xxx
        /// - Format: 2 octets: U16
        /// - Field names: UnsignedValue
        class TDpt7: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            uint16_t FieldUnsignedValue = 0;
        };
    }
}
