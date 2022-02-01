#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Type "Date"
        ///
        /// - ID: 11.001
        /// - Format: 3 octets: r3U5r4U4r1U7
        /// - Field names: Day Month Year
        class TDpt11: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            uint32_t Day{1};
            uint32_t Month{1};
            uint32_t Year{0};
        };
    }
}
