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

        void Send(const TTelegram& telegram) override;

        void Start();
        void Stop();

    private:
        std::unique_ptr<knx::TKnxConnection> KnxdConnection;

        void KnxdConnectProcessing();
        void KnxdReceiveProcessing();

        void HandleLoopError(const std::string& what);

        std::string KnxServerUrl;

        std::atomic<bool> IsStarted{false};
        std::unique_ptr<std::thread> Worker;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
}
