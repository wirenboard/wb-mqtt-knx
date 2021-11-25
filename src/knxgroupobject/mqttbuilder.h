#pragma once
#include "igroupobject.h"
#include "imqttbuilder.h"
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    namespace object
    {
        /// IGroupObjectMqttBuilder implementation
        class TGroupObjectMqttBuilder: public IGroupObjectMqttBuilder
        {
        public:
            TGroupObjectMqttBuilder() = delete;

            /// Constructor
            /// \param pMqttDeviceDriver wb-mqtt Device Driver
            /// \param errorLogger error logger instance
            explicit TGroupObjectMqttBuilder(WBMQTT::PDeviceDriver pMqttDeviceDriver, WBMQTT::TLogger& errorLogger);

            void LinkDevice(const std::string& id, const std::string& name) override;
            PGroupObject Create(const TGroupObjectMqttParameter& parameter) override;
            void RemoveUnusedControls() override;
            void Clear() override;

        private:
            WBMQTT::PDeviceDriver MqttDeviceDriver;
            std::vector<WBMQTT::PLocalDevice> MqttDeviceList;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
