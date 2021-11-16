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
    class TConfigurator
    {
    public:
        explicit TConfigurator(const std::string& configPath,
                               const std::string& schemaPath,
                               std::shared_ptr<object::IGroupObjectMqttBuilder> groupObjectBuilder);

        void Configure(IKnxGroupObjectController& controller);

    private:
        std::unique_ptr<Json::Value> ConfigRoot;
        std::unique_ptr<Json::Value> SchemaRoot;
        std::shared_ptr<object::IGroupObjectMqttBuilder> GroupObjectBuilder;
    };
}
