#pragma once
#include "knxgroupaddress.h"
#include "knxgroupobject/base.h"
#include "knxgroupobject/ifactory.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    class TKnxGroupObjectController: public ISubscriber<TTelegram>, public object::ISenderGroupObject
    {
    public:
        explicit TKnxGroupObjectController() = delete;

        explicit TKnxGroupObjectController(std::shared_ptr<knx::ISender<TTelegram>> pSender): Sender(std::move(pSender))
        {}

        bool AddGroupObject(const TKnxGroupAddress& address, const object::IGroupObjectFactory& objectFactory);
        bool RemoveGroupObject(const TKnxGroupAddress& address);

        virtual ~TKnxGroupObjectController() = default;

    private:
        void Notify(const TTelegram& knxTelegram) override;

        void Send(const object::TGroupObjectTransaction& transaction) const override;

        std::shared_ptr<knx::ISender<TTelegram>> Sender;
        std::map<TKnxGroupAddress, std::shared_ptr<object::TGroupObjectBase>> GroupObjectList;
    };
}
