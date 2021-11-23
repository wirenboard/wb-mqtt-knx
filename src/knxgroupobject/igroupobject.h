#pragma once

#include "../isender.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include <memory>

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

        class IGroupObject
        {
        public:
            virtual void KnxNotify(const TGroupObjectTransaction& transaction) = 0;

            virtual void SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                      std::shared_ptr<ISenderGroupObject> sender) = 0;

            virtual ~IGroupObject() = default;
        };
    }
}
