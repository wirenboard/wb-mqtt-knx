#pragma once

#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types B1
        ///
        /// - ID: 1.xxx
        /// - Format: 1 bit: B1
        /// - Field names: b
        class TDpt1: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            bool FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            bool FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            bool B{false};
        };
    }
}
