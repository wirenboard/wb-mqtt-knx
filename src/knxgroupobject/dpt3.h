#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types B1U3
        ///
        /// - ID: 3.xxx
        /// - Format: 4 bit: B1U
        /// - Field names: c StepCode
        class TDpt3: public IDpt
        {
        public:
            TDatapointId GetId() const override;
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            bool FieldC{false};
            uint8_t FieldStepCode{0};
        };
    }
}
