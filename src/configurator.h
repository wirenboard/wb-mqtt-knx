#pragma once

#include "iknxgroupobjectcontroller.h"
#include "knxexception.h"
#include "knxgroupobject/imqttbuilder.h"
#include <algorithm>
#include <utility>
#include <wblib/json_utils.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    namespace configurator
    {
        /// Group object controller configuration function depending on wb-mqtt-knx config
        /// \param controller Group Object Controller instance
        /// \param configPath wb-mqtt-knx config path
        /// \param schemaPath wb-mqtt-knx schema path
        /// \param groupObjectBuilder Group Object builder
        void ConfigureObjectController(IKnxGroupObjectController& controller,
                                       const std::string& configPath,
                                       const std::string& schemaPath,
                                       object::IGroupObjectMqttBuilder& groupObjectBuilder);
    }
}
