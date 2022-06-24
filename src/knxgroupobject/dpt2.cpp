#include "dpt2.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt2::getDescriptor() const
{
    return {{"c", "switch", 0, 1}, {"v", "switch", 0, 1}};
}

void TDpt2::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            FieldC = value.As<bool>();
            break;
        case 1:
            FieldV = value.As<bool>();
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt2::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 1) {
        FieldV = payload[0] & 0x01;
        FieldC = payload[0] & (1 << 1);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt2::ToKnx()
{
    return {static_cast<uint8_t>((FieldV ? 0x01 : 0x00) | (FieldC ? (1 << 1) : 0x00))};
}

std::vector<WBMQTT::TAny> TDpt2::ToMqtt()
{
    return {FieldC, FieldV};
}

TDatapointId TDpt2::GetId() const
{
    return TDatapointId{2};
}