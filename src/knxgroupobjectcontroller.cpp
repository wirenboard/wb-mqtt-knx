#include "knxgroupobjectcontroller.h"

using namespace knx;

TKnxGroupObjectController::TKnxGroupObjectController(std::shared_ptr<knx::ISender<TTelegram>> pSender)
    : Sender(std::move(pSender))
{}

bool TKnxGroupObjectController::AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                               const std::shared_ptr<object::IGroupObject>& groupObject)
{
    if (groupObject) {
        groupObject->SetKnxSender(groupAddress, shared_from_this());
        return GroupObjectList.insert({groupAddress, groupObject}).second;
    }
    return false;
}

bool TKnxGroupObjectController::RemoveGroupObject(const TKnxGroupAddress& address)
{
    if (GroupObjectList.erase(address)) {
        return true;
    }
    return false;
}

void TKnxGroupObjectController::Notify(const TTelegram& knxTelegram)
{
    if (knxTelegram.IsGroupAddressed()) {
        TKnxGroupAddress address(knxTelegram.GetReceiverAddress());
        auto groupObject = GroupObjectList[address];
        if (groupObject) {
            groupObject->KnxNotify({address, knxTelegram.Tpdu().GetAPCI(), knxTelegram.Tpdu().GetPayload()});
        }
    }
}

void TKnxGroupObjectController::Send(const object::TGroupObjectTransaction& transaction) const
{
    TTelegram knxTelegram{};

    knxTelegram.SetGroupAddressedFlag(true);
    knxTelegram.SetReceiverAddress(transaction.Address.GetEibAddress());
    knxTelegram.Tpdu().SetCommunicationType(telegram::TCommunicationType::UDP);
    knxTelegram.Tpdu().SetAPCI(transaction.Apci);
    knxTelegram.Tpdu().SetPayload(transaction.Payload);

    Sender->Send(knxTelegram);
}
