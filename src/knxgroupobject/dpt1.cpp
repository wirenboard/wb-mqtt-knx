#include "dpt1.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt1::getDescriptor() const
{
    std::vector<DptDescriptorField> descriptor = {{"b", "switch", 0, 1}};
    return descriptor;
}

void TDpt1::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        B = value.As<bool>();
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt1::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 1) {
        B = payload[0] & 0x01;
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt1::ToKnx()
{
    auto val = B ? 0x01 : 0x00;
    return {static_cast<uint8_t>(val)};
}

std::vector<WBMQTT::TAny> TDpt1::ToMqtt()
{
    return {B};
}

TDatapointId TDpt1::GetId() const
{
    return TDatapointId{1};
}
