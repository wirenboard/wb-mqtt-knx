#include "knxgroupaddress.h"

using namespace knx;

namespace
{
    // For the first generation of KNX devices, the maximum address of the main group was 15
    constexpr auto MAIN_GROUP_MAX = 31U;

    constexpr auto MIDDLE_GROUP_MAX = 7U;
    constexpr auto SUB_GROUP_TRIPLE_MAX = 255U;
    constexpr auto SUB_GROUP_DOUBLE_MAX = 2047U;

    // For the first generation of KNX devices, the maximum address was: 0x7FFF
    constexpr auto EIB_ADDRESS_MAX = 0xFFFFU;

    constexpr auto GROUP_ADDRESS_ERROR_MESSAGE = "Invalid KNX Group Address: ";
}

TKnxGroupAddress::TKnxGroupAddress(uint32_t main, uint32_t middle, uint32_t sub)
{
    Init(main, middle, sub);
}

TKnxGroupAddress::TKnxGroupAddress(uint32_t main, uint32_t sub)
{
    Init(main, sub);
}

TKnxGroupAddress::TKnxGroupAddress(const std::string& str)
{
    auto tokens = WBMQTT::StringSplit(str, "/");

    if (tokens.size() == 3) {
        Init(static_cast<uint32_t>(std::stoi(tokens[0])),
             static_cast<uint32_t>(std::stoi(tokens[1])),
             static_cast<uint32_t>(std::stoi(tokens[2])));
    } else if (tokens.size() == 2) {
        Init(static_cast<uint32_t>(std::stoi(tokens[0])), static_cast<uint32_t>(std::stoi(tokens[1])));
    } else if (tokens.size() == 1) {
        Init(static_cast<uint32_t>(std::stoi(tokens[0])));
    } else {
        wb_throw(knx::TKnxException, GROUP_ADDRESS_ERROR_MESSAGE + str);
    }
}

void TKnxGroupAddress::Init(uint32_t main, uint32_t middle, uint32_t sub)
{
    if ((main > MAIN_GROUP_MAX) || (middle > MIDDLE_GROUP_MAX) || (sub > SUB_GROUP_TRIPLE_MAX))
        wb_throw(knx::TKnxException,
                 GROUP_ADDRESS_ERROR_MESSAGE + std::to_string(main) + "/" + std::to_string(middle) + "/" +
                     std::to_string(sub));

    MainGroup = main;
    MiddleGroup = middle;
    SubGroup = sub;
}

void TKnxGroupAddress::Init(uint32_t main, uint32_t sub)
{
    if ((main > MAIN_GROUP_MAX) || (sub > SUB_GROUP_DOUBLE_MAX))
        wb_throw(knx::TKnxException, GROUP_ADDRESS_ERROR_MESSAGE + std::to_string(main) + "/" + std::to_string(sub));
    MainGroup = main;
    MiddleGroup = (sub >> 8) & 0x07;
    SubGroup = sub & 0xFF;
}

void TKnxGroupAddress::Init(uint32_t address)
{
    if ((address > EIB_ADDRESS_MAX))
        wb_throw(knx::TKnxException, GROUP_ADDRESS_ERROR_MESSAGE + std::to_string(address));

    MainGroup = (address >> 11) & 0x1F;
    MiddleGroup = (address >> 8) & 0x07;
    SubGroup = address & 0xFF;
}

TKnxGroupAddress::TKnxGroupAddress(eibaddr_t eibAddress)
{
    Init(eibAddress);
}

eibaddr_t TKnxGroupAddress::GetEibAddress() const
{
    return ((MainGroup & 0x1F) << 11) | ((MiddleGroup & 0x07) << 8) | (SubGroup & 0xFF);
}

bool TKnxGroupAddress::operator<(const TKnxGroupAddress& rhs) const
{
    return GetEibAddress() < rhs.GetEibAddress();
}

bool TKnxGroupAddress::operator==(const TKnxGroupAddress& rhs) const
{
    return ((MainGroup == rhs.MainGroup) && (MiddleGroup == rhs.MiddleGroup) && (SubGroup == rhs.SubGroup));
}

bool TKnxGroupAddress::operator!=(const TKnxGroupAddress& rhs) const
{
    return !(*this == rhs);
}

uint32_t TKnxGroupAddress::GetMainGroup() const
{
    return MainGroup;
}

uint32_t TKnxGroupAddress::GetMiddleGroup() const
{
    return MiddleGroup;
}

uint32_t TKnxGroupAddress::GetSubGroup() const
{
    return SubGroup;
}

std::string TKnxGroupAddress::ToString() const
{
    std::stringstream ss;
    ss << MainGroup << "/" << MiddleGroup << "/" << SubGroup;
    return ss.str();
}
