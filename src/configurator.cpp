#include "configurator.h"

using namespace knx;

void Configurator::ConfigureObjectController(IKnxGroupObjectController& controller,
                                             object::IGroupObjectMqttBuilder& groupObjectBuilder)
{
    auto devices = ConfigRoot["devices"];
    auto readPoolInterval = std::chrono::milliseconds(ConfigRoot["readPoolInterval"].asInt());

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
            auto enableReadPool = control["enableReadPool"].asBool();

            auto groupObject = groupObjectBuilder.Create({dataPointStr, controlIdStr, controlNameStr, isReadOnlyBool});
            controller.AddGroupObject(knx::TKnxGroupAddress{groupAddressStr},
                                      groupObject,
                                      enableReadPool ? readPoolInterval : std::chrono::milliseconds::zero());
        }

        groupObjectBuilder.RemoveUnusedControls();
    }
}

Configurator::Configurator(const std::string& configPath, const std::string& schemaPath)
{
    ConfigRoot = WBMQTT::JSON::Parse(configPath);
    SchemaRoot = WBMQTT::JSON::Parse(schemaPath);

    WBMQTT::JSON::Validate(ConfigRoot, SchemaRoot);
}

bool Configurator::IsDebugEnabled()
{
    return ConfigRoot["debug"].asBool();
}

bool Configurator::IsKnxLegacyDeviceEnabled()
{
    return ConfigRoot["enableLegacyKnxDevice"].asBool();
}
