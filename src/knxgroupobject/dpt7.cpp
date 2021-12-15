#include "dpt7.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;
std::vector<DptDescriptorField> TDpt7::getDescriptor() const
{
    return {{"UnsignedValue", "value", 0, 65535}};
}

void TDpt7::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldUnsignedValue = static_cast<uint16_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt7::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 3) {
        FieldUnsignedValue = payload[1] << 8;
        FieldUnsignedValue |= payload[2];
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt7::ToKnx()
{
    return {0x00, static_cast<uint8_t>(FieldUnsignedValue >> 8), static_cast<uint8_t>(FieldUnsignedValue & 0xFFU)};
}

std::vector<WBMQTT::TAny> TDpt7::ToMqtt()
{
    return {static_cast<double>(FieldUnsignedValue)};
}
