#include "dpt6.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt6::getDescriptor() const
{
    return {{"RelSignedValue", "value", std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max()}};
}

void TDpt6::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        FieldRelSignedValue = static_cast<int8_t>(value.As<double>());
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt6::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        FieldRelSignedValue = static_cast<int8_t>(payload[1]);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt6::ToKnx()
{
    return {0x00, static_cast<uint8_t>(FieldRelSignedValue)};
}

std::vector<WBMQTT::TAny> TDpt6::ToMqtt()
{
    return {static_cast<double>(FieldRelSignedValue)};
}

TDatapointId TDpt6::GetId() const
{
    return TDatapointId{6};
}