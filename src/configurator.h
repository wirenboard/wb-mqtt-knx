#pragma once

#include "knxgroupobject/dpt1.h"
#include "iknxgroupobjectcontroller.h"
#include "mqtt/imqttcontroladapter.h"
#include <wblib/json_utils.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    class TConfigurator
    {
    public:
        explicit TConfigurator(const std::string& configPath,
                               const std::string& schemaPath,
                               std::shared_ptr<mqtt::IMqttDeviceAdapterBuilder> mqttDeviceBuilder)
            : MqttDeviceBuilder(std::move(mqttDeviceBuilder))
        {
            ConfigRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(configPath));
            SchemaRoot = std::make_unique<Json::Value>(WBMQTT::JSON::Parse(schemaPath));

            WBMQTT::JSON::Validate(*ConfigRoot, *SchemaRoot);
        }

        void Configure(IKnxGroupObjectController& controller)
        {
            auto devices = (*SchemaRoot)["devices"];
            for (const auto& device: devices) {
                auto deviceIdStr = device["deviceId"].asString();
                auto deviceNameStr = device["deviceTitle"].asString();

                MqttDeviceBuilder->SetDeviceId(deviceIdStr).SetDeviceName(deviceNameStr);
                auto mqttLocalDeviceAdapter = MqttDeviceBuilder->Create();

                for (const auto& control: device["controls"]) {
                    auto controlIdStr = control["controlId"].asString();
                    auto controlNameStr = control["controlTitle"].asString();

                    auto groupObject =
                        std::make_shared<knx::object::TDpt1>(knx::TKnxGroupAddress{1, 1, 100}, mqttLocalDeviceAdapter);
                    controller.AddGroupObject(groupObject);
                }
            }
        }

    private:
        std::unique_ptr<Json::Value> ConfigRoot;
        std::unique_ptr<Json::Value> SchemaRoot;
        std::shared_ptr<mqtt::IMqttDeviceAdapterBuilder> MqttDeviceBuilder;
    };
}
