#include "dpt14.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include "datapointutils.h"

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
        FieldFloatValue =
            datapointUtils::RawToFloat32((payload[1] << 24) | (payload[2] << 16) | (payload[3] << 8) | (payload[4]));
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt14::ToKnx()
{
    auto rawValue = datapointUtils::FloatToRaw32(FieldFloatValue);
    return {0x00,
            static_cast<uint8_t>(rawValue >> 24),
            static_cast<uint8_t>(rawValue >> 16),
            static_cast<uint8_t>(rawValue >> 8),
            static_cast<uint8_t>(rawValue)};
}

std::vector<WBMQTT::TAny> TDpt14::ToMqtt()
{
    return {static_cast<double>(FieldFloatValue)};
}
