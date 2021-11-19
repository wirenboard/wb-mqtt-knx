#pragma once
#include "igroupobject.h"
#include "imqttbuilder.h"
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{
    namespace object
    {
        class TGroupObjectMqttBuilder: public IGroupObjectMqttBuilder
        {
        public:
            TGroupObjectMqttBuilder() = delete;
            explicit TGroupObjectMqttBuilder(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDeviceDriver,
                                             WBMQTT::TLogger& errorLogger);
            virtual ~TGroupObjectMqttBuilder() = default;

            void LinkDevice(const std::string& id, const std::string& name) override;
            std::shared_ptr<IGroupObject> Create(const TGroupObjectMqttParameter& parameter) override;
            void RemoveUnusedControls() override;
            void Clear() override;

        private:
            std::shared_ptr<WBMQTT::TDeviceDriver> MqttDeviceDriver;
            std::vector<std::shared_ptr<WBMQTT::TLocalDevice>> MqttDeviceList;
            WBMQTT::TLogger& ErrorLogger;
        };
    }
}
