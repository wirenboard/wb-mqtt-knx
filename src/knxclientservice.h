#pragma once

#include "iknxclient.h"
#include "knxconnection.h"
#include "knxtelegram.h"
#include "wblib/log.h"

namespace knx
{
    class TKnxClientService: public IKnxClient
    {
    public:
        explicit TKnxClientService(const std::string& knxServerUrl,
                                   WBMQTT::TLogger& errorLogger,
                                   WBMQTT::TLogger& debugLogger,
                                   WBMQTT::TLogger& infoLogger);

        void Send(const TTelegram& telegram) override;
        void SetOnReceive(const std::function<void(const TTelegram&)>& handler) override;

        void Start();
        void Stop();

        ~TKnxClientService() override = default;

    private:
        void KnxdConnectProcessing();
        void KnxdReceiveProcessing(const knx::TKnxConnection& In);

        void HandleLoopError(const std::string& what);
        void OnReceive(const TTelegram& telegram);

        std::string KnxServerUrl;
        std::function<void(const TTelegram&)> OnReceiveTelegramHandler;

        std::atomic<bool> IsStarted{false};
        std::mutex SetterMutex;
        std::unique_ptr<std::thread> Worker;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
}
