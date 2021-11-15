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
            TGroupObjectMqttBase(const TKnxGroupAddress& address, std::shared_ptr<mqtt::IMqttDeviceAdapter> pMqttDevice)
                : TGroupObjectBase(address),
                  MqttDeviceAdapter(std::move(pMqttDevice))
            {}

            virtual ~TGroupObjectMqttBase() = default;

        private:
            std::shared_ptr<mqtt::IMqttDeviceAdapter> MqttDeviceAdapter;
        };
    }
}
