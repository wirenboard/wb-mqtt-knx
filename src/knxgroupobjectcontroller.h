#pragma once
#include "knxgroupaddress.h"
#include "knxgroupobject/base.h"
#include "knxgroupobject/ifactory.h"
#include "knxtelegram.h"
#include "iknxgroupobjectcontroller.h"
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
        virtual ~TKnxGroupObjectController() = default;

        bool AddGroupObject(const std::shared_ptr<object::TGroupObjectBase>& groupObject) override;
        bool RemoveGroupObject(const TKnxGroupAddress& address) override;

    private:
        void Notify(const TTelegram& knxTelegram) override;

        void Send(const object::TGroupObjectTransaction& transaction) const override;

        std::shared_ptr<knx::ISender<TTelegram>> Sender;
        std::map<TKnxGroupAddress, std::shared_ptr<object::TGroupObjectBase>> GroupObjectList;
    };
}
