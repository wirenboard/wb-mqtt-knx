#pragma once

#include "../isender.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include <memory>

namespace knx
{
    namespace object
    {
        /// Structure for communicating with a group object
        struct TGroupObjectTransaction
        {
            TKnxGroupAddress Address;
            telegram::TApci Apci;
            std::vector<uint8_t> Payload;
        };

        using ISenderGroupObject = ISender<TGroupObjectTransaction>;

        /// Group object for working with a KNX network
        class IGroupObject
        {
        public:
            /// Notifying a group object of a message addressed to it
            /// \param transaction Input message from the KNX network
            virtual void KnxNotify(const TGroupObjectTransaction& transaction) = 0;

            /// Linking the group object to a KNX message transmitter in the network
            /// \param groupAddress the group address to which messages will be sent.
            /// \param sender sender object
            virtual void SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                      std::shared_ptr<ISenderGroupObject> sender) = 0;

            /// Virtual destructor
            virtual ~IGroupObject() = default;
        };
    }
}
