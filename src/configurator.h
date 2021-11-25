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
        void ConfigureObjectController(IKnxGroupObjectController& controller,
                                       const std::string& configPath,
                                       const std::string& schemaPath,
                                       object::IGroupObjectMqttBuilder& groupObjectBuilder);
    }
}
