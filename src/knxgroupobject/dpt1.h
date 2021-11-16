#pragma once
#include "imqttbuilder.h"
#include "mqtt.h"

namespace knx
{
    namespace object
    {
        class TDpt1: public TGroupObjectMqttBase
        {
        public:
            TDpt1(const TGroupObjectMqttParameter& parameter,
                  const std::shared_ptr<mqtt::IMqttDeviceAdapter>& pMqttDevice);

            void KnxNotify(const std::vector<uint8_t>& data) override;

            virtual ~TDpt1();

        private:
            void MqttControlBNotify(const WBMQTT::TAny& value);

            bool B{false};
            std::shared_ptr<mqtt::IMqttControlAdapter> ControlB;
        };
    }
}
