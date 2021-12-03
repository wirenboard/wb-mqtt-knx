#include "knxgroupobjectcontroller.h"

using namespace knx;

TKnxGroupObjectController::TKnxGroupObjectController(PSender<TTelegram> pSender): Sender(std::move(pSender))
{}

bool TKnxGroupObjectController::AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                               const object::PGroupObject& groupObject)
{
    if (groupObject) {
        groupObject->SetKnxSender(groupAddress, shared_from_this());
        return GroupObjectList.insert({groupAddress, groupObject}).second;
    }
    return false;
}

bool TKnxGroupObjectController::RemoveGroupObject(const TKnxGroupAddress& address)
{
    return GroupObjectList.erase(address);
}

void TKnxGroupObjectController::Notify(const TTelegram& knxTelegram)
{
    if (knxTelegram.IsGroupAddressed()) {
        TKnxGroupAddress address(knxTelegram.GetReceiverAddress());

        auto groupObjectIterator = GroupObjectList.find(address);
        if (groupObjectIterator != GroupObjectList.end()) {
            groupObjectIterator->second->KnxNotify(
                {address, knxTelegram.Tpdu().GetAPCI(), knxTelegram.Tpdu().GetPayload()});
        }
    }
}

void TKnxGroupObjectController::Send(const object::TGroupObjectTransaction& transaction)
{
    TTelegram knxTelegram{};

    knxTelegram.SetGroupAddressedFlag(true);
    knxTelegram.SetReceiverAddress(transaction.Address.GetEibAddress());
    knxTelegram.Tpdu().SetCommunicationType(telegram::TCommunicationType::UDP);
    knxTelegram.Tpdu().SetAPCI(transaction.Apci);
    knxTelegram.Tpdu().SetPayload(transaction.Payload);

    Sender->Send(knxTelegram);
}
