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
            TKnxGroupAddress Address;     ///< Destination group address
            telegram::TApci Apci;         ///< Command code for group objects
            std::vector<uint8_t> Payload; ///< KNX telegram payload
        };

        enum class TGroupObjectError
        {
            None = 0,
            SocketError,
            PoolReadTimeoutError
        };

        using ISenderGroupObject = ISender<TGroupObjectTransaction>;
        using PSenderGroupObject = std::shared_ptr<ISenderGroupObject>;

        /// Group object for working with a KNX network
        class IGroupObject
        {
        public:
            /// Notifying a group object of a message addressed to it
            /// \param transaction Input message from the KNX network
            virtual void KnxNotify(const TGroupObjectTransaction& transaction) = 0;

            /// Notification of a group object about an error in the KNX network
            /// \param error Error value
            virtual void KnxError(const TGroupObjectError& error) = 0;

            /// Linking the group object to a KNX message transmitter in the network
            /// \param groupAddress the group address to which messages will be sent.
            /// \param sender sender object
            virtual void SetKnxSender(const knx::TKnxGroupAddress& groupAddress, PSenderGroupObject sender) = 0;

            /// Virtual destructor
            virtual ~IGroupObject() = default;
        };

        using PGroupObject = std::shared_ptr<IGroupObject>;
    }
}
