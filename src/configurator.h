#pragma once

#include "iknxgroupobjectcontroller.h"
#include "knxexception.h"
#include "knxgroupobject/imqttbuilder.h"
#include <algorithm>
#include <utility>
#include <wblib/json_utils.h>
#include <wblib/wbmqtt.h>

namespace
{
    knx::TKnxGroupAddress StringToGroupAddress(const std::string& str)
    {
        auto addr = str;

        auto noSpaceEnd = std::remove(addr.begin(), addr.end(), ' ');
        addr.erase(noSpaceEnd, addr.end());

        auto tokens = WBMQTT::StringSplit(addr, "/");

        if (tokens.size() == 3) {
            return {static_cast<uint32_t>(std::stoi(tokens[0])),
                    static_cast<uint32_t>(std::stoi(tokens[1])),
                    static_cast<uint32_t>(std::stoi(tokens[2]))};
        } else if (tokens.size() == 2) {
            return {static_cast<uint32_t>(std::stoi(tokens[0])), static_cast<uint32_t>(std::stoi(tokens[1]))};
        } else {
            wb_throw(knx::TKnxException, "Configurator: Error parsing of KNX Group Address");
        }
        return {0, 0, 0};
    }
}

namespace knx
{
    class TConfigurator
    {
    public:
        explicit TConfigurator(const std::string& configPath,
                               const std::string& schemaPath,
                               std::shared_ptr<object::IGroupObjectMqttBuilder> groupObjectBuilder)
            : GroupObjectBuilder(std::move(groupObjectBuilder))
        {
            ConfigRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(configPath));
            SchemaRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(schemaPath));

            WBMQTT::JSON::Validate(*ConfigRoot, *SchemaRoot);
        }

        void Configure(IKnxGroupObjectController& controller)
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

                    auto groupObject = GroupObjectBuilder->Create(
                        {StringToGroupAddress(groupAddressStr), dataPointStr, controlIdStr, controlNameStr});
                    controller.AddGroupObject(groupObject);
                }
            }
        }

    private:
        std::unique_ptr<Json::Value> ConfigRoot;
        std::unique_ptr<Json::Value> SchemaRoot;
        std::shared_ptr<object::IGroupObjectMqttBuilder> GroupObjectBuilder;
    };
}
