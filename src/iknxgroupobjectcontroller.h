#pragma once

#include "knxgroupaddress.h"
#include "knxgroupobject/groupobjectsettings.h"
#include "knxgroupobject/igroupobject.h"
#include "knxtelegram.h"
#include <experimental/optional>
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
        /// \param groupObject group object for management
        /// \param settings group object settings
        /// \return The group object has been successfully added to the controller
        virtual bool AddGroupObject(const object::PGroupObject& groupObject, const TGroupObjectSettings& settings) = 0;

        /// Remove group object from controller
        /// \param groupAddress group address associated with a group object
        /// \return The group object has been successfully removed from the controller
        virtual bool RemoveGroupObject(const TKnxGroupAddress& groupAddress) = 0;

        /// Virtual destructor
        virtual ~IKnxGroupObjectController() = default;
    };
}
