#include "dptbaseu8.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;
std::vector<DptDescriptorField> TDptBaseU8::getDescriptor() const
{
    return {{"UnsignedValue", "value", std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}};
}

void TDptBaseU8::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldUnsignedValue = static_cast<uint8_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDptBaseU8::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        FieldUnsignedValue = payload[1];
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDptBaseU8::ToKnx()
{
    return {0x00, FieldUnsignedValue};
}

std::vector<WBMQTT::TAny> TDptBaseU8::ToMqtt()
{
    return {static_cast<double>(FieldUnsignedValue)};
}
