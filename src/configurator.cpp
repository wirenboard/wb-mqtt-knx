#include "configurator.h"

using namespace knx;

void Configurator::ConfigureObjectController(IKnxGroupObjectController& controller,
                                             object::IGroupObjectMqttBuilder& groupObjectBuilder)
{
    auto devices = ConfigRoot["devices"];

    for (const auto& device: devices) {
        auto deviceIdStr = device["deviceId"].asString();
        auto deviceTitleStr = device["deviceTitle"].asString();

        groupObjectBuilder.LinkDevice(deviceIdStr, deviceTitleStr);

        for (const auto& control: device["controls"]) {
            TGroupObjectSettings goSettings;
            goSettings.ControlId = control["controlId"].asString();
            goSettings.ControlTitle = control["controlTitle"].asString();
            goSettings.GroupAddress = TKnxGroupAddress{control["groupAddress"].asString()};
            goSettings.DatapointType = control["dataPointType"].asString();
            goSettings.IsReadOnly = control["readOnly"].asBool();

            if (control.isMember("feedbackGroupAddress") && !control["feedbackGroupAddress"].asString().empty()) {
                goSettings.FeedbackGroupAddress = TKnxGroupAddress{control["feedbackGroupAddress"].asString()};
            }
            goSettings.ReadRequestAfterStart = true;
            goSettings.ReadRequestPollInterval = std::chrono::milliseconds(control["readPollInterval"].asInt());
            goSettings.ReadResponseTimeout = std::chrono::milliseconds(control["readPollTimeout"].asInt());

            auto groupObject = groupObjectBuilder.Create(goSettings);
            controller.AddGroupObject(groupObject, goSettings);
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
