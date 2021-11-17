#pragma once

#include "../mqtt/imqttcontroladapter.h"
#include "../mqtt/imqttdeviceadapter.h"
#include "base.h"
#include "idpt.h"
#include <wblib/wbmqtt.h>

#include <utility>

namespace knx
{
    namespace object
    {
        class TGroupObjectMqtt: public TGroupObjectBase
        {
        public:
            explicit TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                      const std::string& controlId,
                                      const std::string& controlName,
                                      std::shared_ptr<mqtt::IMqttDeviceAdapter> pMqttDevice);

            void KnxNotify(const std::vector<uint8_t>& data) override;

            void MqttNotify(uint32_t index, const WBMQTT::TAny& value);

            virtual ~TGroupObjectMqtt() = default;

        private:
            std::shared_ptr<IDpt> Dpt;
            std::shared_ptr<mqtt::IMqttDeviceAdapter> MqttDeviceAdapter;
            std::vector<std::shared_ptr<mqtt::IMqttControlAdapter>> ControlList;
        };
    }
}
