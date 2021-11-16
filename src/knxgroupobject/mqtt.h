#pragma once

#include "../mqtt/imqttcontroladapter.h"
#include "../mqtt/imqttdeviceadapter.h"
#include "base.h"
#include <wblib/wbmqtt.h>

#include <utility>

namespace knx
{
    namespace object
    {
        class TGroupObjectMqttBase: public TGroupObjectBase
        {
        public:
            explicit TGroupObjectMqttBase(std::shared_ptr<mqtt::IMqttDeviceAdapter> pMqttDevice)
                : MqttDeviceAdapter(std::move(pMqttDevice))
            {}

            virtual ~TGroupObjectMqttBase() = default;

        private:
            std::shared_ptr<mqtt::IMqttDeviceAdapter> MqttDeviceAdapter;
        };
    }
}
