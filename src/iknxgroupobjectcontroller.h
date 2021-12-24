#pragma once

#include "knxgroupaddress.h"
#include "knxgroupobject/igroupobject.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    /// Group object settings
    struct TGroupObjectSettings
    {
        bool ReadRequestAfterStart{false}; ///< Send a read request after the start of the knxd client
        std::chrono::milliseconds ReadRequestPollInterval{0}; ///< Period of sending a read request
        std::chrono::milliseconds ReadResponseTimeout{0};     ///< Time to wait for a response to a read request
                                                              ///< Must be less than the polling period.
    };

    /// Class containing group objects and routing incoming KNX messages to group objects
    class IKnxGroupObjectController
    {
    public:
        /// Add group object to controller
        /// \param groupAddress group address associated with a group object
        /// \param groupObject group object for management
        /// \param settings group object settings
        /// \return The group object has been successfully added to the controller
        virtual bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                    const object::PGroupObject& groupObject,
                                    const TGroupObjectSettings& settings) = 0;

        /// Remove group object from controller
        /// \param groupAddress group address associated with a group object
        /// \return The group object has been successfully removed from the controller
        virtual bool RemoveGroupObject(const TKnxGroupAddress& groupAddress) = 0;

        /// Virtual destructor
        virtual ~IKnxGroupObjectController() = default;
    };
}
