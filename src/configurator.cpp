#include "configurator.h"

void knx::configurator::ConfigureObjectController(IKnxGroupObjectController& controller,
                                                  const std::string& configPath,
                                                  const std::string& schemaPath,
                                                  object::IGroupObjectMqttBuilder& groupObjectBuilder)
{
    auto configRoot = WBMQTT::JSON::Parse(configPath);
    auto schemaRoot = WBMQTT::JSON::Parse(schemaPath);

    WBMQTT::JSON::Validate(configRoot, schemaRoot);

    auto devices = configRoot["devices"];
    for (const auto& device: devices) {
        auto deviceIdStr = device["deviceId"].asString();
        auto deviceTitleStr = device["deviceTitle"].asString();

        groupObjectBuilder.LinkDevice(deviceIdStr, deviceTitleStr);

        for (const auto& control: device["controls"]) {
            auto controlIdStr = control["controlId"].asString();
            auto controlNameStr = control["controlTitle"].asString();
            auto groupAddressStr = control["groupAddress"].asString();
            auto dataPointStr = control["dataPointType"].asString();
            auto isReadOnlyBool = control["readOnly"].asBool();

            auto groupObject = groupObjectBuilder.Create({dataPointStr, controlIdStr, controlNameStr, isReadOnlyBool});
            controller.AddGroupObject(knx::TKnxGroupAddress{groupAddressStr}, groupObject);
        }

        groupObjectBuilder.RemoveUnusedControls();
    }
}
