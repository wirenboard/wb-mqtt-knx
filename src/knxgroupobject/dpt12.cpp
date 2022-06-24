#include "dpt12.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt12::getDescriptor() const
{
    return {{"UnsignedValue", "value", 0, std::numeric_limits<uint32_t>::max()}};
}

void TDpt12::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldUnsignedValue = static_cast<uint32_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt12::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 5) {
        FieldUnsignedValue = payload[1] << 24;
        FieldUnsignedValue |= payload[2] << 16;
        FieldUnsignedValue |= payload[3] << 8;
        FieldUnsignedValue |= payload[4];
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt12::ToKnx()
{
    return {0x00,
            static_cast<uint8_t>(FieldUnsignedValue >> 24),
            static_cast<uint8_t>(FieldUnsignedValue >> 16),
            static_cast<uint8_t>(FieldUnsignedValue >> 8),
            static_cast<uint8_t>(FieldUnsignedValue & 0xFFU)};
}

std::vector<WBMQTT::TAny> TDpt12::ToMqtt()
{
    return {static_cast<double>(FieldUnsignedValue)};
}

TDatapointId TDpt12::GetId() const
{
    return TDatapointId{12};
}