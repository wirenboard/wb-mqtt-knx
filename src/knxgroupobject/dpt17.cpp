#include "dpt17.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt17::getDescriptor() const
{
    return {{"SceneNumber", "value", 0, 63}};
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
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt17::ToKnx()
{
    return {0, static_cast<uint8_t>(SceneNumber & 0x3F)};
}

std::vector<WBMQTT::TAny> TDpt17::ToMqtt()
{
    return {static_cast<double>(SceneNumber)};
}
