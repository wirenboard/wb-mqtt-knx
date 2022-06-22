#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "4-Octet Float Value"
        ///
        /// - ID: 14.xxx
        /// - Format: 4 octets: F32
        /// - Field names: FloatValue
        class TDpt14: public IDpt
        {
        public:
            TDatapointId GetId() const override;
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            float FieldFloatValue = 0;
        };
    }
}
