#include "base.h"

using namespace knx::object;

TGroupObjectBase::TGroupObjectBase(const knx::TKnxGroupAddress& address): SelfKnxAddress(address)
{}

void TGroupObjectBase::KnxSend(const telegram::TApci& apci, const std::vector<uint8_t>& data) const
{
    Sender->Send({SelfKnxAddress, apci, data});
}

void TGroupObjectBase::Notify(const TGroupObjectTransaction& transaction)
{
    if ((transaction.Apci == telegram::TApci::GroupValueWrite) ||
        (transaction.Apci == telegram::TApci::GroupValueResponse)) {
        KnxNotify(transaction.Payload);
    }
}

knx::TKnxGroupAddress TGroupObjectBase::GetKnxAddress() const
{
    return SelfKnxAddress;
}

void TGroupObjectBase::SetKnxSender(std::shared_ptr<ISenderGroupObject> sender)
{
    Sender = std::move(sender);
}
