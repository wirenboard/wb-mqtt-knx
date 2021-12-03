#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "2-Octet Float Value"
        ///
        /// - ID: 9.xxx
        /// - Format: 2 octets: F16
        /// - Field names: FloatValue
        class TDpt9: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            double FieldFloatValue{false};
        };
    }
}
