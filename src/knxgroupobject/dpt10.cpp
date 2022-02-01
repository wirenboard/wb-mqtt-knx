#include "dpt10.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt10::getDescriptor() const
{
    return {{"Day", "value", 0, 7}, {"Hour", "value", 0, 23}, {"Minutes", "value", 0, 59}, {"Seconds", "value", 0, 59}};
}
void TDpt10::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            Day = static_cast<uint32_t>(value.As<double>());
            break;
        case 1:
            Hour = static_cast<uint32_t>(value.As<double>());
            break;
        case 2:
            Minutes = static_cast<uint32_t>(value.As<double>());
            break;
        case 3:
            Seconds = static_cast<uint32_t>(value.As<double>());
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt10::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 4) {
        Day = (payload[1] >> 5) & 0x07;
        Hour = payload[1] & 0x1F;
        Minutes = payload[2] & 0x3F;
        Seconds = payload[3] & 0x3F;
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt10::ToKnx()
{
    return {0,
            static_cast<uint8_t>(((Day & 0x07) << 5) | (Hour & 0x1F)),
            static_cast<uint8_t>(Minutes & 0x3F),
            static_cast<uint8_t>(Seconds & 0x3F)};
}

std::vector<WBMQTT::TAny> TDpt10::ToMqtt()
{
    return {static_cast<double>(Day),
            static_cast<double>(Hour),
            static_cast<double>(Minutes),
            static_cast<double>(Seconds)};
}
