#include "base.h"

using namespace knx::object;

void TGroupObjectBase::KnxSend(const std::vector<uint8_t>& data) const
{
    Sender->Send({SelfKnxAddress, telegram::TApci::GroupValueWrite, data});
}

void TGroupObjectBase::Notify(const TGroupObjectTransaction& transaction)
{
    if ((transaction.Apci == telegram::TApci::GroupValueWrite) ||
        (transaction.Apci == telegram::TApci::GroupValueResponse)) {
        KnxNotify(transaction.Payload);
    }
}

void TGroupObjectBase::SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                    std::shared_ptr<ISenderGroupObject> sender)
{
    SelfKnxAddress = groupAddress;
    Sender = std::move(sender);
}
