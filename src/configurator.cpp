#include "configurator.h"

using namespace knx;

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

TConfigurator::TConfigurator(const std::string& configPath,
                             const std::string& schemaPath,
                             std::shared_ptr<object::IGroupObjectMqttBuilder> groupObjectBuilder)
    : GroupObjectBuilder(std::move(groupObjectBuilder))
{
    ConfigRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(configPath));
    SchemaRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(schemaPath));

    WBMQTT::JSON::Validate(*ConfigRoot, *SchemaRoot);
}

void TConfigurator::Configure(IKnxGroupObjectController& controller)
{
    if (GroupObjectBuilder == nullptr) {
        wb_throw(TKnxException, "Configurator: GroupObjectBuilder is nullptr");
    }

    auto devices = (*ConfigRoot)["devices"];
    for (const auto& device: devices) {
        auto deviceIdStr = device["deviceId"].asString();
        auto deviceNameStr = device["deviceTitle"].asString();

        GroupObjectBuilder->LinkDevice(deviceIdStr, deviceNameStr);

        for (const auto& control: device["controls"]) {
            auto controlIdStr = control["controlId"].asString();
            auto controlNameStr = control["controlTitle"].asString();
            auto groupAddressStr = control["groupAddress"].asString();
            auto dataPointStr = control["dataPointType"].asString();
            auto isReadOnlyBool = control["readOnly"].asBool();

            auto groupObject = GroupObjectBuilder->Create({dataPointStr, controlIdStr, controlNameStr, isReadOnlyBool});
            controller.AddGroupObject(StringToGroupAddress(groupAddressStr), groupObject);
        }

        GroupObjectBuilder->RemoveUnusedControls();
    }
}
