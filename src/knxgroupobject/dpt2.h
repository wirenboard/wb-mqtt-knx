#pragma once
#include "imqttbuilder.h"
#include "mqtt.h"

namespace knx
{
    namespace object
    {
        class TDpt2: public TGroupObjectMqttBase
        {
        public:
            TDpt2(const TGroupObjectMqttParameter& parameter,
                  const std::shared_ptr<mqtt::IMqttDeviceAdapter>& pMqttDevice);

            void KnxNotify(const std::vector<uint8_t>& data) override;

            virtual ~TDpt2();

        private:
            void MqttControlCNotify(const WBMQTT::TAny& value);
            void MqttControlVNotify(const WBMQTT::TAny& value);

            void Send();

            bool FieldC{false};
            bool FieldV{false};
            std::shared_ptr<mqtt::IMqttControlAdapter> ControlC;
            std::shared_ptr<mqtt::IMqttControlAdapter> ControlV;
        };
    }
}
