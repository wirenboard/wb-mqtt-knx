#include "dpt9.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include "datapointutils.h"
#include <cmath>

using namespace knx::object;

std::vector<DptDescriptorField> TDpt9::getDescriptor() const
{
    return {{"FloatValue", "value", -671088.64, 670760.96}};
}

void TDpt9::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldFloatValue = value.As<double>();
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt9::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 3) {
        FieldFloatValue = datapointUtils::RawToFloat16(payload[1] << 8 | payload[2]);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt9::ToKnx()
{
    auto dValue = datapointUtils::FloatToRaw16(FieldFloatValue);
    return {0, static_cast<uint8_t>(dValue >> 8), static_cast<uint8_t>(dValue & 0xFF)};
}

std::vector<WBMQTT::TAny> TDpt9::ToMqtt()
{
    return {FieldFloatValue};
}
