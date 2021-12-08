#pragma once
#include "iknxgroupobjectcontroller.h"
#include "isubscriber.h"
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
                                     public ISubscriber<TTelegram>,
                                     public ISubscriber<TTickTimerEvent>,
                                     public object::ISenderGroupObject,
                                     public std::enable_shared_from_this<TKnxGroupObjectController>
    {
    public:
        explicit TKnxGroupObjectController() = delete;
        explicit TKnxGroupObjectController(knx::PSender<TTelegram> pSender, std::chrono::milliseconds tickInterval);

        bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                            const object::PGroupObject& groupObject,
                            std::chrono::milliseconds pollInterval) override;
        bool RemoveGroupObject(const TKnxGroupAddress& address) override;

    private:
        struct TGroupObjectListItem
        {
            object::PGroupObject groupObject;
            uint32_t pollInterval{0};
            uint32_t counter{0};
            bool RequestedRead = false;
        };

        void Notify(const TTelegram& knxTelegram) override;

        void Notify(const TTickTimerEvent& timerEvent) override;

        void Send(const object::TGroupObjectTransaction& transaction) override;

        PSender<TTelegram> Sender;
        std::chrono::milliseconds TickInterval;
        std::map<TKnxGroupAddress, TGroupObjectListItem> GroupObjectList;
    };
}
