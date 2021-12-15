#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "2-Octet Signed Value"
        ///
        /// - ID: 8.xxx
        /// - Format: 2 octet: V16
        /// - Field names: SignedValue
        class TDpt8: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            int16_t FieldSignedValue = 0;
        };
    }
}
