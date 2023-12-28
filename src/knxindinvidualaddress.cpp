#include "knxindividualaddress.h"

using namespace knx;

namespace
{
    constexpr auto AREA_MAX = 15U;
    constexpr auto LINE_MAX = 15U;
    constexpr auto DEVICE_ADDRESS_MAX = 255U;

    constexpr auto ERROR_MESSAGE = "Invalid KNX Individual Address: ";
}

TKnxIndividualAddress::TKnxIndividualAddress(uint32_t area, uint32_t line, uint32_t deviceAddress)
{
    Init(area, line, deviceAddress);
}

TKnxIndividualAddress::TKnxIndividualAddress(const std::string& str, char delimiter)
{
    auto tokens = WBMQTT::StringSplit(str, delimiter);

    if (tokens.size() == 3) {
        Init(static_cast<uint32_t>(std::stoi(tokens[0])),
             static_cast<uint32_t>(std::stoi(tokens[1])),
             static_cast<uint32_t>(std::stoi(tokens[2])));
    } else {
        wb_throw(knx::TKnxException, ERROR_MESSAGE + str);
    }
}

TKnxIndividualAddress::TKnxIndividualAddress(eibaddr_t eibAddress)
{
    Area = (eibAddress >> 12) & 0xF;
    Line = (eibAddress >> 8) & 0xF;
    DeviceAddress = eibAddress & 0xFF;
}

void TKnxIndividualAddress::Init(uint32_t area, uint32_t line, uint32_t deviceAddress)
{
    if ((area > AREA_MAX) || (line > LINE_MAX) || (deviceAddress > DEVICE_ADDRESS_MAX))
        wb_throw(knx::TKnxException,
                 ERROR_MESSAGE + std::to_string(area) + "." + std::to_string(line) + "." +
                     std::to_string(deviceAddress));

    Area = area;
    Line = line;
    DeviceAddress = deviceAddress;
}

eibaddr_t TKnxIndividualAddress::GetEibAddress() const
{
    return ((Area & 0xF) << 12) | ((Line & 0xF) << 8) | (DeviceAddress & 0xFF);
}

bool TKnxIndividualAddress::operator<(const TKnxIndividualAddress& rhs) const
{
    return GetEibAddress() < rhs.GetEibAddress();
}

bool TKnxIndividualAddress::operator==(const TKnxIndividualAddress& rhs) const
{
    return ((Area == rhs.Area) && (Line == rhs.Line) && (DeviceAddress == rhs.DeviceAddress));
}

bool TKnxIndividualAddress::operator!=(const TKnxIndividualAddress& rhs) const
{
    return !(*this == rhs);
}

uint32_t TKnxIndividualAddress::GetArea() const
{
    return Area;
}

uint32_t TKnxIndividualAddress::GetLine() const
{
    return Line;
}

uint32_t TKnxIndividualAddress::GetDeviceAddress() const
{
    return DeviceAddress;
}

std::string TKnxIndividualAddress::ToString(char delimiter) const
{
    std::stringstream ss;
    ss << Area << delimiter << Line << delimiter << DeviceAddress;
    return ss.str();
}
