#include "dpt5.h"
#include "../knxexception.h"
#include <cmath>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt5::getDescriptor() const
{
    return {{"UnsignedValue", "value", 0, 255}};
}

bool TDpt5::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldUnsignedValue = static_cast<uint8_t>(value.As<double>());
        return true;
    }
    return false;
}

bool TDpt5::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        FieldUnsignedValue = payload[1];
        return true;
    }
    return false;
}

std::vector<uint8_t> TDpt5::ToKnx()
{
    return {0x00, FieldUnsignedValue};
}

std::vector<WBMQTT::TAny> TDpt5::ToMqtt()
{
    return {static_cast<double>(FieldUnsignedValue)};
}
