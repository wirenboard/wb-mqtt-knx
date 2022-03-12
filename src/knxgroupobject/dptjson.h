#pragma once

#include "dptjsonfield.h"
#include "idpt.h"
#include <bitset>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        /// \brief JSON datapoint class
        class TDptJson: public IDpt
        {
        public:
            /// Configuring JSON Datapoint by adding data fields
            /// \param field Datapoint field
            /// \param BitBeginPosition most significant bit position
            void AddField(const TDptJsonField& field, uint32_t BitBeginPosition);

            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;

            void FromKnx(const std::vector<uint8_t>& payload) override;

            std::vector<uint8_t> ToKnx() override;

            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            struct FieldWithPosition
            {
                TDptJsonField JsonField;
                uint32_t BitBeginPosition{};
            };
            std::vector<FieldWithPosition> JsonFieldList;
            uint32_t PayloadBytesNum{};
        };
    }
}