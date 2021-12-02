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
    /// \brief Class for wb-mqtt-knx service configuration
    class Configurator
    {
    public:
        /// Constructor
        /// \param configPath wb-mqtt-knx config path
        /// \param schemaPath wb-mqtt-knx schema path
        Configurator(const std::string& configPath, const std::string& schemaPath);

        /// Group object controller configuration function depending on wb-mqtt-knx config
        /// \param controller Group Object Controller instance
        /// \param groupObjectBuilder Group Object builder
        void ConfigureObjectController(IKnxGroupObjectController& controller,
                                       object::IGroupObjectMqttBuilder& groupObjectBuilder);

        /// Is the option enabled in the config
        /// \return Debug is enable
        bool IsDebugEnabled();

        /// Is the option enabled in the config
        /// \return KNX Legacy Device is enable
        bool IsKnxLegacyDeviceEnabled();

    private:
        Json::Value ConfigRoot;
        Json::Value SchemaRoot;
    };
}
