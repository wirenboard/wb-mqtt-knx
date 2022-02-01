#include "dpt18.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt18::getDescriptor() const
{
    return {{"Control", "switch", 0, 1}, {"SceneNumber", "value", 1, 64}};
}

void TDpt18::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            Control = value.As<bool>() ? EControlField::Learn : EControlField::Activate;
            break;
        case 1:
            SceneNumber = static_cast<uint32_t>(value.As<double>());
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt18::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        Control = (payload[1] & 0x80) ? EControlField::Learn : EControlField::Activate;
        SceneNumber = payload[1] & 0x3F;
        //  DPT_SceneControl allows numbering the scene from 0 to 63.
        //  KNX Association recommends displaying these scene numbers in ETS,
        //  other software and controllers numbered from 1 to 64, this is,
        //  with an offset of 1 compared to the actual transmitted value.
        SceneNumber += 1;
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt18::ToKnx()
{
    return {0, static_cast<uint8_t>((Control == EControlField::Learn ? (1 << 7) : 0) | ((SceneNumber - 1) & 0x3F))};
}

std::vector<WBMQTT::TAny> TDpt18::ToMqtt()
{
    return {static_cast<bool>(Control == EControlField::Learn), static_cast<double>(SceneNumber)};
}
