#pragma once

#include "groupobjectsettings.h"
#include "idpt.h"
#include "igroupobject.h"
#include <utility>
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    namespace object
    {
        /// IGroupObject implementation
        class TGroupObjectMqtt: public IGroupObject
        {
        public:
            /// Constructor
            /// \param pDpt Data point strategy
            /// \param controlId Control Id prefix
            /// \param controlTitle Control title
            /// \param isReadOnly Control read only flag
            /// \param pMqttDevice wb-mqtt Device
            /// \param errorLogger error logger instance
            explicit TGroupObjectMqtt(PDpt pDpt,
                                      const knx::TGroupObjectSettings& settings,
                                      WBMQTT::PLocalDevice pMqttDevice,
                                      WBMQTT::TLogger& errorLogger);

            void KnxNotify(const TGroupObjectTransaction& transaction) override;
            void KnxNotifyEvent(const TKnxEvent& event) override;
            void SetKnxSender(const TKnxGroupAddress& groupAddress, PSenderGroupObject sender) override;

        private:
            void MqttNotify(WBMQTT::PControl& pControl, uint32_t index, const WBMQTT::TAny& value);

            TKnxGroupAddress SelfKnxAddress;
            PSenderGroupObject KnxSender;
            PDpt Dpt;
            TGroupObjectSettings Settings;
            WBMQTT::PLocalDevice MqttLocalDevice;
            std::vector<WBMQTT::PControl> ControlList;
            std::mutex DptExchangeMutex;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
