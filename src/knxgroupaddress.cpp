#include "knxgroupaddress.h"

using namespace knx;

TKnxGroupAddress::TKnxGroupAddress(uint32_t main, uint32_t middle, uint32_t sub)
    : MainGroup(main),
      MiddleGroup(middle),
      SubGroup(sub)
{}

TKnxGroupAddress::TKnxGroupAddress(uint32_t main, uint32_t sub)
{
    MainGroup = main;
    MiddleGroup = (sub >> 8) & 0x03;
    SubGroup = sub & 0xff;
}

TKnxGroupAddress::TKnxGroupAddress(eibaddr_t eibAddress)
{
    MainGroup = (eibAddress >> 11) & 0x0F;
    MiddleGroup = (eibAddress >> 8) & 0x03;
    SubGroup = eibAddress & 0xFF;
}

eibaddr_t TKnxGroupAddress::GetEibAddress() const
{
    return ((MainGroup & 0x0F) << 11) | ((MiddleGroup & 0x03) << 8) | (SubGroup & 0x0F);
}

bool TKnxGroupAddress::operator<(const TKnxGroupAddress& rhs) const
{
    return GetEibAddress() < rhs.GetEibAddress();
}
bool TKnxGroupAddress::operator==(const TKnxGroupAddress& rhs) const
{
    return ((MainGroup == rhs.MainGroup) && (MiddleGroup == rhs.MiddleGroup) && (SubGroup == rhs.SubGroup));
}
