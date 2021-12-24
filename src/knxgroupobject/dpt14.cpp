#include "dpt14.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <netinet/in.h>

namespace
{
    static_assert(std::numeric_limits<float>::is_iec559, "Float is not IEC 559/IEEE 754 compliant");

    union
    {
        uint32_t integerValue;
        float floatValue;
    } floatConverter;

    float PayloadToFloat(const std::vector<uint8_t>& vec)
    {
        floatConverter.integerValue = (vec[1] << 24) | (vec[2] << 16) | (vec[3] << 8) | (vec[4]);

        return floatConverter.floatValue;
    }

    std::vector<uint8_t> FloatToPayload(float value)
    {
        floatConverter.floatValue = value;

        return {0x00,
                static_cast<uint8_t>(floatConverter.integerValue >> 24),
                static_cast<uint8_t>(floatConverter.integerValue >> 16),
                static_cast<uint8_t>(floatConverter.integerValue >> 8),
                static_cast<uint8_t>(floatConverter.integerValue)};
    }
}

using namespace knx::object;
std::vector<DptDescriptorField> TDpt14::getDescriptor() const
{
    return {{"SignedValue", "value", std::numeric_limits<float>::min(), std::numeric_limits<float>::max()}};
}

void TDpt14::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldFloatValue = static_cast<float>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt14::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 5) {
        FieldFloatValue = PayloadToFloat(payload);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt14::ToKnx()
{
    return FloatToPayload(FieldFloatValue);
}

std::vector<WBMQTT::TAny> TDpt14::ToMqtt()
{
    return {static_cast<double>(FieldFloatValue)};
}
