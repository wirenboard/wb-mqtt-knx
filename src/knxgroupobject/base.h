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
            explicit TGroupObjectBase(const TKnxGroupAddress& address): SelfKnxAddress(address)
            {}

            virtual TKnxGroupAddress GetKnxAddress()
            {
                return SelfKnxAddress;
            }

            virtual void SetKnxSender(std::shared_ptr<ISenderGroupObject> sender)
            {
                Sender = sender;
            }

            void Notify(const TGroupObjectTransaction& args) override;

        protected:
            void KnxSend(const telegram::TApci&, const std::vector<uint8_t>& data) const;
            virtual void KnxNotify(const std::vector<uint8_t>& payload) = 0;

        private:
            const TKnxGroupAddress SelfKnxAddress;
            std::shared_ptr<ISenderGroupObject> Sender;
        };
    }
}
