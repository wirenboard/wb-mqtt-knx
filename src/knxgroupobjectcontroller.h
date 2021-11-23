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
        explicit TKnxGroupObjectController(std::shared_ptr<knx::ISender<TTelegram>> pSender);

        bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                            const std::shared_ptr<object::IGroupObject>& groupObject) override;
        bool RemoveGroupObject(const TKnxGroupAddress& address) override;

    private:
        void Notify(const TTelegram& knxTelegram) override;

        void Send(const object::TGroupObjectTransaction& transaction) override;

        std::shared_ptr<knx::ISender<TTelegram>> Sender;
        std::map<TKnxGroupAddress, std::shared_ptr<object::IGroupObject>> GroupObjectList;
    };
}
