#pragma once
#include "../mqtt/mqttdeviceadapter.h"
#include "imqttbuilder.h"

namespace knx
{
    namespace object
    {
        class TGroupObjectMqttBuilder: public IGroupObjectMqttBuilder
        {
        public:
            TGroupObjectMqttBuilder() = delete;
            explicit TGroupObjectMqttBuilder(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDeviceDriver);
            virtual ~TGroupObjectMqttBuilder() = default;

            void LinkDevice(const std::string& id, const std::string& name) override;
            std::shared_ptr<TGroupObjectBase> Create(const TGroupObjectMqttParameter& parameter) override;

        private:
            std::shared_ptr<WBMQTT::TDeviceDriver> MqttDeviceDriver;
            std::shared_ptr<mqtt::MqttDeviceAdapter> MqttDevice;
        };
    }
}
