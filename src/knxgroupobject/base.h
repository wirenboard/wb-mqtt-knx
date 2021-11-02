#pragma once

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
            explicit TGroupObjectBase() = delete;

            explicit TGroupObjectBase(const TKnxGroupAddress& address, const ISenderGroupObject& sender)
                : SelfKnxAddress(address),
                  Sender(sender)
            {}

        protected:
            void Send(const telegram::TApci&, const std::vector<uint8_t>& data) const;

        private:
            const TKnxGroupAddress SelfKnxAddress;
            const ISenderGroupObject& Sender;
        };
    }
}
