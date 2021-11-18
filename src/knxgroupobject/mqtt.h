#pragma once

#include "../mqtt/imqttcontroladapter.h"
#include "../mqtt/imqttdeviceadapter.h"
#include "base.h"
#include "idpt.h"
#include "igroupobject.h"
#include <wblib/wbmqtt.h>

#include <utility>

namespace knx
{
    namespace object
    {
        class TGroupObjectMqtt: public IGroupObject
        {
        public:
            explicit TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                      const std::string& controlId,
                                      const std::string& controlName,
                                      std::shared_ptr<mqtt::IMqttDeviceAdapter> pMqttDevice);

            void MqttNotify(uint32_t index, const WBMQTT::TAny& value);

            void KnxNotify(const TGroupObjectTransaction& transaction) override;

            void SetKnxSender(const TKnxGroupAddress& groupAddress,
                              std::shared_ptr<ISenderGroupObject> sender) override;

            virtual ~TGroupObjectMqtt() = default;

        private:
            TKnxGroupAddress SelfKnxAddress;
            std::shared_ptr<ISenderGroupObject> KnxSender;

            std::shared_ptr<IDpt> Dpt;
            std::shared_ptr<mqtt::IMqttDeviceAdapter> MqttDeviceAdapter;
            std::vector<std::shared_ptr<mqtt::IMqttControlAdapter>> ControlList;
        };
    }
}
