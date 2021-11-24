#pragma once

#pragma once
#include "knxgroupaddress.h"
#include "knxgroupobject/igroupobject.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    /// Class containing group objects and routing incoming KNX messages to group objects
    class IKnxGroupObjectController
    {
    public:
        /// Add group object to controller
        /// \param groupAddress group address associated with a group object
        /// \param groupObject group object for management
        /// \return The group object has been successfully added to the controller
        virtual bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                    const std::shared_ptr<object::IGroupObject>& groupObject) = 0;

        /// Remove group object from controller
        /// \param groupAddress group address associated with a group object
        /// \return The group object has been successfully removed from the controller
        virtual bool RemoveGroupObject(const TKnxGroupAddress& groupAddress) = 0;

        /// Virtual destructor
        virtual ~IKnxGroupObjectController() = default;
    };
}
