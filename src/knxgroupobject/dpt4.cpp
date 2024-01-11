#include "dpt4.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <cmath>

using namespace knx::object;

std::vector<DptDescriptorField> TDpt4::getDescriptor() const
{
    return {{"CharacterValue", "text", 0, 255}};
}

void TDpt4::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        if (value.As<std::string>().empty()) {
            FieldCharacterValue = '\0';
        } else {
            FieldCharacterValue = value.As<std::string>()[0];
        }
    } else {
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
    }
}

void TDpt4::FromKnx(const std::vector<uint8_t>& payload)
{
    if (payload.size() == 2) {
        FieldCharacterValue = payload[1];
    } else {
        wb_throw(TKnxException, datapointError::KNX_INVALID_PAYLOAD_SIZE);
    }
}

std::vector<uint8_t> TDpt4::ToKnx()
{
    return {0x00, static_cast<uint8_t>(FieldCharacterValue)};
}

std::vector<WBMQTT::TAny> TDpt4::ToMqtt()
{
    std::string textValue;
    if (FieldCharacterValue != '\0') {
        textValue = FieldCharacterValue;
    }
    return {textValue};
}

TDatapointId TDpt4::GetId() const
{
    return TDatapointId{4};
}