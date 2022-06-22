#include "dpt3.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt3::getDescriptor() const
{
    return {{"c", "switch", 0, 1}, {"StepCode", "value", 0, 7}};
}

void TDpt3::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            FieldC = value.As<bool>();
            break;
        case 1:
            FieldStepCode = static_cast<uint8_t>(value.As<double>());
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX + std::to_string(controlIndex));
    }
}

void TDpt3::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 1) {
        FieldStepCode = payload[0] & 0x07;
        FieldC = payload[0] & (1 << 3);
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt3::ToKnx()
{
    return {static_cast<uint8_t>((FieldStepCode & 0x07) | (FieldC ? (1 << 3) : 0x00))};
}

std::vector<WBMQTT::TAny> TDpt3::ToMqtt()
{
    return {FieldC, static_cast<double>(FieldStepCode)};
}

TDatapointId TDpt3::GetId() const
{
    return TDatapointId{3};
}