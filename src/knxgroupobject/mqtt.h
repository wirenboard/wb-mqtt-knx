#pragma once

#include "idpt.h"
#include "igroupobject.h"
#include <utility>
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    namespace object
    {
        class TGroupObjectMqtt: public IGroupObject
        {
        public:
            explicit TGroupObjectMqtt(PDpt pDpt,
                                      const std::string& controlId,
                                      const std::string& controlTitle,
                                      bool isReadOnly,
                                      WBMQTT::PLocalDevice pMqttDevice,
                                      WBMQTT::TLogger& errorLogger);

            void MqttNotify(WBMQTT::PControl& pControl, uint32_t index, const WBMQTT::TAny& value);

            void KnxNotify(const TGroupObjectTransaction& transaction) override;

            void SetKnxSender(const TKnxGroupAddress& groupAddress, PSenderGroupObject sender) override;

        private:
            TKnxGroupAddress SelfKnxAddress;
            PSenderGroupObject KnxSender;
            PDpt Dpt;
            WBMQTT::PLocalDevice MqttLocalDevice;
            std::vector<WBMQTT::PControl> ControlList;
            std::mutex DptExchangeMutex;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
