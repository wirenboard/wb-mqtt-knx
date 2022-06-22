#include "dpt13.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt13::getDescriptor() const
{
    return {{"SignedValue", "value", std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()}};
}

void TDpt13::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldSignedValue = static_cast<int32_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt13::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 5) {
        uint32_t value;
        value = static_cast<uint32_t>(payload[1] << 24);
        value |= static_cast<uint32_t>(payload[2] << 16);
        value |= static_cast<uint32_t>(payload[3] << 8);
        value |= static_cast<uint32_t>(payload[4]);
        FieldSignedValue = static_cast<int32_t>(value);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt13::ToKnx()
{
    return {0x00,
            static_cast<uint8_t>(FieldSignedValue >> 24),
            static_cast<uint8_t>(FieldSignedValue >> 16),
            static_cast<uint8_t>(FieldSignedValue >> 8),
            static_cast<uint8_t>(FieldSignedValue)};
}

std::vector<WBMQTT::TAny> TDpt13::ToMqtt()
{
    return {static_cast<double>(FieldSignedValue)};
}

TDatapointId TDpt13::GetId() const
{
    return TDatapointId{13};
}
