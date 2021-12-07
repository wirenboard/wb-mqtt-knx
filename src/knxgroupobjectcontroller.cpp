#include "knxgroupobjectcontroller.h"

using namespace knx;

TKnxGroupObjectController::TKnxGroupObjectController(PSender<TTelegram> pSender, std::chrono::milliseconds tickInterval)
    : Sender(std::move(pSender)),
      TickInterval(tickInterval)
{}

bool TKnxGroupObjectController::AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                               const object::PGroupObject& groupObject,
                                               std::chrono::milliseconds pollInterval)
{
    if (groupObject) {
        groupObject->SetKnxSender(groupAddress, shared_from_this());
        return GroupObjectList
            .insert({groupAddress, {groupObject, static_cast<uint32_t>(pollInterval.count() / TickInterval.count())}})
            .second;
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
            groupObjectIterator->second.groupObject->KnxNotify(
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

void TKnxGroupObjectController::Notify(const TTickTimerEvent& timerEvent)
{
    if (timerEvent == TTickTimerEvent::TimeIsUp) {
        for (auto& itemPair: GroupObjectList) {
            auto& address = itemPair.first;
            auto& item = itemPair.second;
            if (item.pollInterval) {
                if (item.counter > 0) {
                    --item.counter;
                } else {
                    item.counter = item.pollInterval;
                    Send({address, telegram::TApci::GroupValueRead, {0}});
                }
            }
        }
    }
}
