#pragma once

#include "../knxutils.h"
#include "idpt.h"

namespace knx
{
    namespace object
    {
        /// \brief Raw KNX payload data.
        ///
        /// Array of bytes as a text string. The first data byte is the first byte in the APDU.
        /// <br>Example:
        ///        - for a control input data: "34 0x34 0b11011"
        ///        - for a control output data: "0x01 0x03 0x50"
        class TDptRaw: public IDpt
        {
        public:
            TDatapointId GetId() const override;

            std::vector<DptDescriptorField> getDescriptor() const override;

            void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) override;

            void FromKnx(const std::vector<uint8_t>& payload) override;

            std::vector<uint8_t> ToKnx() override;

            std::vector<WBMQTT::TAny> ToMqtt() override;

        private:
            std::vector<uint8_t> RawData = {0};
        };
    }
}