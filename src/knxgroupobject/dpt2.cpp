#include "dpt2.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt2::getDescriptor() const
{
    return {{"c", "switch", 0, 1}, {"v", "switch", 0, 1}};
}

std::vector<uint8_t> TDpt2::mqttToKnx(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            FieldC = value.As<bool>();
            break;
        case 1:
            FieldV = value.As<bool>();
            break;
        default:;
    }
    return {static_cast<uint8_t>((FieldV ? 0x01 : 0x00) | (FieldC ? (1 << 1) : 0x00))};
}

std::vector<WBMQTT::TAny> TDpt2::knxToMqtt(const std::vector<uint8_t>& payload)
{
    FieldV = payload[0] & 0x01;
    FieldC = payload[0] & (1 << 1);
    return {FieldC, FieldV};
}
