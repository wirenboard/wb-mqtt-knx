#include "dpt17.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt17::getDescriptor() const
{
    return {{"SceneNumber", "value", 1, 64}};
}

void TDpt17::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            SceneNumber = static_cast<uint32_t>(value.As<double>());
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt17::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
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

std::vector<uint8_t> TDpt17::ToKnx()
{
    return {0, static_cast<uint8_t>((SceneNumber - 1) & 0x3F)};
}

std::vector<WBMQTT::TAny> TDpt17::ToMqtt()
{
    return {static_cast<double>(SceneNumber)};
}
