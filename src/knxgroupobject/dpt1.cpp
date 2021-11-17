#include "dpt1.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt1::getDescriptor() const
{
    std::vector<DptDescriptorField> descriptor = {{"b", "switch", 0, 1}};
    return descriptor;
}

std::vector<uint8_t> TDpt1::mqttToKnx(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        B = value.As<bool>();
        auto val = B ? 0x01 : 0x00;
        return {static_cast<uint8_t>(val)};
    }
    return {};
}

std::vector<WBMQTT::TAny> TDpt1::knxToMqtt(const std::vector<uint8_t>& payload)
{
    B = payload[0] & 0x01;
    return {B};
}
