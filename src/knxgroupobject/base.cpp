#include "base.h"

using namespace knx::object;

void TGroupObjectBase::Send(const telegram::TApci& apci, const std::vector<uint8_t>& data) const
{
    Sender.Send({SelfKnxAddress, apci, data});
}
