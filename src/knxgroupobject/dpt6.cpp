#include "dpt6.h"
#include "../knxexception.h"

using namespace knx::object;
std::vector<DptDescriptorField> TDpt6::getDescriptor() const
{
    return {{"RelSignedValue", "value", -128, 127}};
}

bool TDpt6::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldRelSignedValue = static_cast<int8_t>(value.As<double>());
        return true;
    }
    return false;
}

bool TDpt6::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        FieldRelSignedValue = static_cast<int8_t>(payload[1]);
        return true;
    }
    return false;
}

std::vector<uint8_t> TDpt6::ToKnx()
{
    return {0x00, static_cast<uint8_t>(FieldRelSignedValue)};
}

std::vector<WBMQTT::TAny> TDpt6::ToMqtt()
{
    return {static_cast<double>(FieldRelSignedValue)};
}
