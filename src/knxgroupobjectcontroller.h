#pragma once
#include "iknxgroupobjectcontroller.h"
#include "isubscriber.h"
#include "knxevent.h"
#include "knxgroupaddress.h"
#include "knxgroupobject/igroupobject.h"
#include "knxtelegram.h"
#include "ticktimer.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    class TKnxGroupObjectController: public IKnxGroupObjectController,
                                     public ISubscriber<TKnxEvent, TTelegram>,
                                     public ISubscriber<TTickTimerEvent>,
                                     public object::ISenderGroupObject,
                                     public std::enable_shared_from_this<TKnxGroupObjectController>
    {
    public:
        explicit TKnxGroupObjectController() = delete;
        explicit TKnxGroupObjectController(knx::PSender<TTelegram> pSender, std::chrono::milliseconds tickInterval);

        bool AddGroupObject(const object::PGroupObject& groupObject, const TGroupObjectSettings& settings) override;
        bool RemoveGroupObject(const TKnxGroupAddress& address) override;

    private:
        struct TGroupObjectListItem
        {
            object::PGroupObject GroupObject;
            TGroupObjectSettings Settings;
            uint32_t PollInterval{0};
            uint32_t Timeout{0};
            uint32_t Counter{0};
            uint32_t TimeoutCounter{0};
            std::atomic<bool> StartTimeoutTimer{false};
        };

        void Notify(const TKnxEvent& event, const TTelegram& knxTelegram) override;

        void Notify(const TTickTimerEvent& timerEvent) override;

        void Send(const object::TGroupObjectTransaction& transaction) override;

        PSender<TTelegram> Sender;
        std::chrono::milliseconds TickInterval;
        std::map<TKnxGroupAddress, std::shared_ptr<TGroupObjectListItem>> GroupObjectList;
    };
}
