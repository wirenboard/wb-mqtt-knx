#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types B2
        ///
        /// - ID: 2.xxx
        /// - Format: 2 bit: B2
        /// - Field names: c v
        class TDpt2: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            bool FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            bool FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            bool FieldC{false};
            bool FieldV{false};
        };
    }
}
