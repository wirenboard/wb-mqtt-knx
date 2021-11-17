#include "dpt9.h"
#include <cmath>

using namespace knx::object;

// TODO Need check negative numbers

std::vector<DptDescriptorField> TDpt9::getDescriptor() const
{
    return {{"FloatValue", "value", -671088.64, 670760.96}};
}

std::vector<uint8_t> TDpt9::mqttToKnx(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    FieldFloatValue = value.As<double>();

    if (controlIndex == 0) {
        auto mantissa = static_cast<int32_t>(std::round(FieldFloatValue * 100)); // std::floor(FieldFloatValue * 100);
        uint32_t exp = 0;
        while ((mantissa < -2048) || (mantissa > 2047)) {
            mantissa >>= 1;
            ++exp;
        }

        auto dValue = (static_cast<int16_t>(mantissa) & 0x87FF) | ((exp & 0x0F) << 11);

        return {0, static_cast<uint8_t>(dValue >> 8), static_cast<uint8_t>(dValue & 0xFF)};
    }
    return {};
}

std::vector<WBMQTT::TAny> TDpt9::knxToMqtt(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 3) {
        bool isNegative = payload[1] & (1 << 7);
        auto mantissa = static_cast<int16_t>((isNegative ? (0x1F << 11) : 0) |
                                             (static_cast<uint16_t>(payload[1] & 0x07) << 8) | payload[2]);
        uint32_t exp = (payload[1] >> 3) & 0x0F;
        FieldFloatValue = (static_cast<double>(mantissa) * 0.01) * std::pow(2, exp);
        return {FieldFloatValue};
    }

    return {};
}
