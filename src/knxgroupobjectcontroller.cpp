#include "knxgroupobjectcontroller.h"

using namespace knx;

bool TKnxGroupObjectController::AddGroupObject(const TKnxGroupAddress& address,
                                               const object::IGroupObjectFactory& objectFactory)
{
    return GroupObjectList.insert({address, objectFactory.Create(address, *this)}).second;
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
            groupObject->Notify({address, knxTelegram.Tpdu().GetAPCI(), knxTelegram.Tpdu().GetPayload()});
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
