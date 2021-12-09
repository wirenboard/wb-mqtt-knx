#pragma once

#include "isender.h"
#include "isubscriber.h"
#include "knxevent.h"
#include "knxtelegram.h"
#include <wblib/log.h>
#include <wblib/wbmqtt.h>

namespace knx
{

    class TKnxLegacyDevice: public ISubscriber<TKnxEvent, TTelegram>
    {
    public:
        explicit TKnxLegacyDevice(WBMQTT::PDeviceDriver pMqttDriver,
                                  knx::PSender<TTelegram> pKnxTelegramSender,
                                  WBMQTT::TLogger& errorLogger,
                                  WBMQTT::TLogger& debugLogger,
                                  WBMQTT::TLogger& infoLogger);

        void Deinit();

        void Notify(const TKnxEvent& event, const TTelegram& t) override;

    private:
        WBMQTT::PDeviceDriver DeviceDriver;
        knx::PSender<TTelegram> KnxTelegramSender;
        WBMQTT::PLocalDevice LocalDevice;
        WBMQTT::PControl Control;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
}
