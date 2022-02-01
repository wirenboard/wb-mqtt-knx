#include "dpt11.h"
#include "../knxexception.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDpt11::getDescriptor() const
{
    return {{"Day", "value", 1, 31}, {"Month", "value", 1, 12}, {"Year", "value", 0, 99}};
}

void TDpt11::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    switch (controlIndex) {
        case 0:
            Day = static_cast<uint32_t>(value.As<double>());
            break;
        case 1:
            Month = static_cast<uint32_t>(value.As<double>());
            break;
        case 2:
            Year = static_cast<uint32_t>(value.As<double>());
            break;
        default:
            wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt11::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 4) {
        Day = payload[1] & 0x1F;
        Month = payload[2] & 0x0F;
        Year = payload[3] & 0x7F;
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt11::ToKnx()
{
    return {0, static_cast<uint8_t>(Day & 0x1F), static_cast<uint8_t>(Month & 0x0F), static_cast<uint8_t>(Year & 0x7F)};
}

std::vector<WBMQTT::TAny> TDpt11::ToMqtt()
{
    return {static_cast<double>(Day), static_cast<double>(Month), static_cast<double>(Year)};
}
