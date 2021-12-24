#include "knxgroupobjectcontroller.h"

using namespace knx;

TKnxGroupObjectController::TKnxGroupObjectController(PSender<TTelegram> pSender, std::chrono::milliseconds tickInterval)
    : Sender(std::move(pSender)),
      TickInterval(tickInterval)
{}

bool TKnxGroupObjectController::AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                               const object::PGroupObject& groupObject,
                                               const TGroupObjectSettings& settings)
{
    if (groupObject && (GroupObjectList.find(groupAddress) == GroupObjectList.end())) {
        groupObject->SetKnxSender(groupAddress, shared_from_this());

        auto pItem = std::make_unique<TGroupObjectListItem>();
        pItem->GroupObject = groupObject;
        pItem->Settings = settings;
        pItem->PollInterval = static_cast<uint32_t>(settings.ReadRequestPollInterval.count() / TickInterval.count());
        pItem->Timeout = static_cast<uint32_t>(settings.ReadResponseTimeout.count() / TickInterval.count());
        pItem->Counter = pItem->PollInterval;

        return GroupObjectList.emplace(groupAddress, std::move(pItem)).second;
    }
    return false;
}

bool TKnxGroupObjectController::RemoveGroupObject(const TKnxGroupAddress& address)
{
    return GroupObjectList.erase(address);
}

void TKnxGroupObjectController::Notify(const TKnxEvent& event, const TTelegram& knxTelegram)
{
    if (event == TKnxEvent::KnxdSocketConnected) {
        for (auto& itemPair: GroupObjectList) {
            auto& address = itemPair.first;
            auto& item = itemPair.second;
            if (item->Settings.ReadRequestAfterStart) {
                Send({address, telegram::TApci::GroupValueRead, {0}});
            }
        }
    } else if (event != TKnxEvent::ReceivedTelegram) {
        return;
    }

    if (knxTelegram.IsGroupAddressed()) {
        TKnxGroupAddress address(knxTelegram.GetReceiverAddress());

        auto groupObjectIterator = GroupObjectList.find(address);
        if (groupObjectIterator != GroupObjectList.end()) {
            auto& item = groupObjectIterator->second;
            auto apci = knxTelegram.Tpdu().GetAPCI();
            if (apci == telegram::TApci::GroupValueResponse) {
                item->StartTimeoutTimer = false;
            }
            item->GroupObject->KnxNotify({address, knxTelegram.Tpdu().GetAPCI(), knxTelegram.Tpdu().GetPayload()});
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

            if (item->Timeout && item->StartTimeoutTimer) {
                if (item->TimeoutCounter > 0) {
                    --item->TimeoutCounter;
                } else {
                    item->StartTimeoutTimer = false;
                    item->GroupObject->KnxNotifyEvent(TKnxEvent::PollReadTimeoutError);
                }
            }

            if (item->PollInterval) {
                if (item->Counter > 0) {
                    --item->Counter;
                } else {
                    item->Counter = item->PollInterval;
                    Send({address, telegram::TApci::GroupValueRead, {0}});
                    item->StartTimeoutTimer = true;
                    item->TimeoutCounter = item->Timeout;
                }
            }
        }
    }
}
