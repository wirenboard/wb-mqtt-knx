#pragma once

#include "iknxclient.h"
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{

    class TKnxDevice
    {
    public:
        explicit TKnxDevice(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDriver,
                            std::shared_ptr<knx::IKnxClient> pKnxClient,
                            WBMQTT::TLogger& errorLogger,
                            WBMQTT::TLogger& debugLogger,
                            WBMQTT::TLogger& infoLogger);

        void Deinit();

        ~TKnxDevice() = default;

    private:
        std::shared_ptr<WBMQTT::TDeviceDriver> DeviceDriver;
        WBMQTT::PDriverEventHandlerHandle EventHandlerHandle;
        std::shared_ptr<knx::IKnxClient> KnxClient;
        std::shared_ptr<WBMQTT::TLocalDevice> LocalDevice;
        std::shared_ptr<WBMQTT::TControl> Control;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
} // namespace knx

