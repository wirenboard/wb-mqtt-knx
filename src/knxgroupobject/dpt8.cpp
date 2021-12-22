#include "dpt8.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt8::getDescriptor() const
{
    return {{"SignedValue", "value", std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()}};
}

void TDpt8::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldSignedValue = static_cast<int16_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt8::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 3) {
        uint16_t value;
        value = static_cast<uint16_t>(payload[1] << 8);
        value |= static_cast<uint16_t>(payload[2]);

        FieldSignedValue = static_cast<int16_t>(value);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt8::ToKnx()
{
    return {0x00, static_cast<uint8_t>(FieldSignedValue >> 8), static_cast<uint8_t>(FieldSignedValue & 0xFFU)};
}

std::vector<WBMQTT::TAny> TDpt8::ToMqtt()
{
    return {static_cast<double>(FieldSignedValue)};
}
