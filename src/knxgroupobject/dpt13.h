#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "4-Octet Signed Value"
        ///
        /// - ID: 13.xxx
        /// - Format: 4 octets: V32
        /// - Field names: SignedValue
        class TDpt13: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            int32_t FieldSignedValue = 0;
        };
    }
}
