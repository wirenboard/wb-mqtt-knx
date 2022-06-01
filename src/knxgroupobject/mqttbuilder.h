#pragma once
#include "idptbuilder.h"
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
            /// \param dptJsonBuilder JSON datapoint builder
            /// \param errorLogger error logger instance
            explicit TGroupObjectMqttBuilder(WBMQTT::PDeviceDriver pMqttDeviceDriver,
                                             object::IDptBuilder& dptJsonBuilder,
                                             WBMQTT::TLogger& errorLogger);

            void LinkDevice(const std::string& id, const std::string& name) override;
            PGroupObject Create(const TGroupObjectSettings& settings) override;
            void RemoveUnusedControls() override;
            void Clear() override;

        private:
            WBMQTT::PDeviceDriver MqttDeviceDriver;
            object::IDptBuilder& DptJsonBuilder;
            std::vector<WBMQTT::PLocalDevice> MqttDeviceList;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
