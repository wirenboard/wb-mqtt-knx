#pragma once
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "Character Set"
        ///
        /// - ID: 4.xxx
        /// - Format: 8 bit: A8
        /// - Field names: Character
        class TDpt4: public IDpt
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;
            void FromKnx(const std::vector<uint8_t>& payload) override;
            std::vector<uint8_t> ToKnx() override;
            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            char FieldCharacterValue = 0;
        };
    }
}
