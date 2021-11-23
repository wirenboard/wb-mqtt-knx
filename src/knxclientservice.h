#pragma once

#include "isender.h"
#include "knxconnection.h"
#include "knxtelegram.h"
#include "observer.h"
#include "wblib/log.h"
#include <list>

namespace knx
{
    class TKnxClientService: public ISender<TTelegram>, public TObserver<TTelegram>
    {
    public:
        explicit TKnxClientService(const std::string& knxServerUrl,
                                   WBMQTT::TLogger& errorLogger,
                                   WBMQTT::TLogger& debugLogger,
                                   WBMQTT::TLogger& infoLogger);

        void Send(const TTelegram& telegram) const override;

        void Start();
        void Stop();

    private:
        void KnxdConnectProcessing();
        void KnxdReceiveProcessing(const knx::TKnxConnection& In);

        void HandleLoopError(const std::string& what);

        std::string KnxServerUrl;
        std::function<void(const TTelegram&)> OnReceiveTelegramHandler;

        std::atomic<bool> IsStarted{false};
        std::unique_ptr<std::thread> Worker;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
}
