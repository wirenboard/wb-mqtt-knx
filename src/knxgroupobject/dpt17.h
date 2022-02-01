#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Type Scene Number
        ///
        /// - ID: 17.001
        /// - Format: 1 octet: r2U6
        /// - Field names: SceneNumber
        class TDpt17: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            uint32_t SceneNumber{0};
        };
    }
}
