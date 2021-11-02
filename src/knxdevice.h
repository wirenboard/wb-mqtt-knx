#pragma once

#include "isender.h"
#include "isubscriber.h"
#include "knxtelegram.h"
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{

    class TKnxDevice: public ISubscriber<TTelegram>
    {
    public:
        explicit TKnxDevice(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDriver,
                            std::shared_ptr<knx::ISender<TTelegram>> pKnxTelegramSender,
                            WBMQTT::TLogger& errorLogger,
                            WBMQTT::TLogger& debugLogger,
                            WBMQTT::TLogger& infoLogger);

        void Deinit();

        virtual ~TKnxDevice() = default;

        void Notify(const TTelegram& t) override;

    private:
        std::shared_ptr<WBMQTT::TDeviceDriver> DeviceDriver;
        WBMQTT::PDriverEventHandlerHandle EventHandlerHandle;
        std::shared_ptr<knx::ISender<TTelegram>> KnxTelegramSender;
        std::shared_ptr<WBMQTT::TLocalDevice> LocalDevice;
        std::shared_ptr<WBMQTT::TControl> Control;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
}
