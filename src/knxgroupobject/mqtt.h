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
            explicit TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                      const std::string& controlId,
                                      const std::string& controlTitle,
                                      bool isReadOnly,
                                      std::shared_ptr<WBMQTT::TLocalDevice> pMqttDevice,
                                      WBMQTT::TLogger& errorLogger);

            void MqttNotify(const std::string& deviceId,
                            const std::string& controlId,
                            uint32_t index,
                            const WBMQTT::TAny& value);

            void KnxNotify(const TGroupObjectTransaction& transaction) override;

            void SetKnxSender(const TKnxGroupAddress& groupAddress,
                              std::shared_ptr<ISenderGroupObject> sender) override;

        private:
            TKnxGroupAddress SelfKnxAddress;
            std::shared_ptr<ISenderGroupObject> KnxSender;

            std::shared_ptr<IDpt> Dpt;
            std::shared_ptr<WBMQTT::TLocalDevice> MqttLocalDevice;
            std::vector<std::shared_ptr<WBMQTT::TControl>> ControlList;
            std::mutex DptExchangeMutex;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
