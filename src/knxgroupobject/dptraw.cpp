#include "dptraw.h"
#include "../knxexception.h"
#include "../knxtelegram.h"
#include "datapointerror.h"

using namespace knx::object;

std::vector<DptDescriptorField> TDptRaw::getDescriptor() const
{
    return {{"RawValue", "text"}};
}

void TDptRaw::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
{
    if (controlIndex == 0) {
        std::stringstream ss(value.As<std::string>());
        std::string dataStr;
        RawData.clear();
        uint32_t size = 0;
        while (ss >> dataStr) {
            RawData.push_back(utils::StringValueToByte(dataStr));
            ++size;
            if (size > knx::TTpdu::MaxPayloadSize)
                wb_throw(TKnxException, "Telegram payload is too long.");
        }
    } else
        wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
}

void TDptRaw::FromKnx(const std::vector<uint8_t>& payload)
{
    if (!payload.empty()) {
        RawData = payload;
    } else
        wb_throw(TKnxException, "KNX payload is empty");
}

std::vector<uint8_t> TDptRaw::ToKnx()
{
    return RawData;
}

std::vector<WBMQTT::TAny> TDptRaw::ToMqtt()
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (const auto& payloadByte: RawData) {
        ss << "0x" << std::setw(2) << static_cast<uint32_t>(payloadByte) << " ";
    }

    return {ss.str()};
}
