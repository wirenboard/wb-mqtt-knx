#pragma once
#include "iknxgroupobjectcontroller.h"
#include "isubscriber.h"
#include "knxgroupaddress.h"
#include "knxgroupobject/igroupobject.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    class TKnxGroupObjectController: public IKnxGroupObjectController,
                                     public ISubscriber<TTelegram>,
                                     public object::ISenderGroupObject,
                                     public std::enable_shared_from_this<TKnxGroupObjectController>
    {
    public:
        explicit TKnxGroupObjectController() = delete;
        explicit TKnxGroupObjectController(knx::PSender<TTelegram> pSender);

        bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                            const object::PGroupObject& groupObject) override;
        bool RemoveGroupObject(const TKnxGroupAddress& address) override;

    private:
        void Notify(const TTelegram& knxTelegram) override;

        void Send(const object::TGroupObjectTransaction& transaction) override;

        PSender<TTelegram> Sender;
        std::map<TKnxGroupAddress, object::PGroupObject> GroupObjectList;
    };
}
