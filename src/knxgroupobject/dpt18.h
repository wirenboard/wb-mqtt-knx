#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Type Scene Control
        ///
        /// - ID: 18.001
        /// - Format: 1 octet: B1r1U6
        /// - Field names: Control SceneNumber
        class TDpt18: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            enum class EControlField
            {
                Activate = 0,
                Learn = 1
            };
            uint32_t SceneNumber{1};
            EControlField Control{EControlField::Activate};
        };
    }
}
