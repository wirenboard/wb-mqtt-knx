#pragma once

#include <utility>

#include "../isender.h"
#include "../isubscriber.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include "../observer.h"

namespace knx
{
    namespace object
    {
        struct TGroupObjectTransaction
        {
            TKnxGroupAddress Address;
            telegram::TApci Apci;
            std::vector<uint8_t> Payload;
        };

        using ISenderGroupObject = ISender<TGroupObjectTransaction>;

        class TGroupObjectBase: public ISubscriber<TGroupObjectTransaction>
        {
        public:
            explicit TGroupObjectBase() = default;

            virtual void SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                      std::shared_ptr<ISenderGroupObject> sender);

            void Notify(const TGroupObjectTransaction& args) override;

        protected:
            void KnxSend(const std::vector<uint8_t>& data) const;
            virtual void KnxNotify(const std::vector<uint8_t>& payload) = 0;

        private:
            TKnxGroupAddress SelfKnxAddress;
            std::shared_ptr<ISenderGroupObject> Sender;
        };
    }
}
