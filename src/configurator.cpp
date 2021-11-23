#include "configurator.h"

namespace
{
    knx::TKnxGroupAddress StringToGroupAddress(const std::string& str)
    {
        auto tokens = WBMQTT::StringSplit(str, "/");

        if (tokens.size() == 3) {
            return {static_cast<uint32_t>(std::stoi(tokens[0])),
                    static_cast<uint32_t>(std::stoi(tokens[1])),
                    static_cast<uint32_t>(std::stoi(tokens[2]))};
        } else if (tokens.size() == 2) {
            return {static_cast<uint32_t>(std::stoi(tokens[0])), static_cast<uint32_t>(std::stoi(tokens[1]))};
        } else {
            wb_throw(knx::TKnxException, "Configurator: Error parsing of KNX Group Address");
        }
    }
}

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
            controller.AddGroupObject(StringToGroupAddress(groupAddressStr), groupObject);
        }

        groupObjectBuilder.RemoveUnusedControls();
    }
}
