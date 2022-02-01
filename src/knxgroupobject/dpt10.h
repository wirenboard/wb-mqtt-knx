#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Type "Time"
        ///
        /// - ID: 10.001
        /// - Format: 3 octets: N3U5r2U6r2U6
        /// - Field names: Day Hour Minutes Seconds
        class TDpt10: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            uint32_t Day{0};
            uint32_t Hour{0};
            uint32_t Minutes{0};
            uint32_t Seconds{0};
        };
    }
}
