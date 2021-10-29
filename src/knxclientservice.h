#ifndef WB_MQTT_KNX_KNXCLIENTSERVICE_H
#define WB_MQTT_KNX_KNXCLIENTSERVICE_H

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
        void ReceiveLoop();
        void HandleLoopError(const std::string& what);
        void ReceiveProcessing(const knx::TKnxConnection& In);
        void OnReceive(const TTelegram& telegram) const;

        std::string KnxServerUrl;
        std::function<void(const TTelegram&)> OnReceiveTelegramHandler;

        std::atomic<bool> IsStarted{false};
        std::mutex IsStartedMutex;
        mutable std::mutex SetterMutex;
        std::unique_ptr<std::thread> Worker;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
} // namespace knx

#endif // WB_MQTT_KNX_KNXCLIENTSERVICE_H
