#ifndef WB_MQTT_KNX_KNXCLIENTSERVICE_H
#define WB_MQTT_KNX_KNXCLIENTSERVICE_H

#include "iknxclient.h"
#include "knxtelegram.h"
#include "wblib/log.h"

namespace knx
{
    class TKnxClientService: public IKnxClient
    {
        // wrapper for EIBConnection
        class TKnxConnection
        {
        public:
            explicit TKnxConnection(const std::string& knxServerUrl)
            {
                Connection = EIBSocketURL(knxServerUrl.c_str());
            }

            EIBConnection* GetEIBConnection() const
            {
                return Connection;
            }

            bool operator!() const
            {
                return !Connection;
            }

            ~TKnxConnection()
            {
                EIBClose(Connection);
            }

        private:
            EIBConnection* Connection;
        };

    public:
        explicit TKnxClientService(std::string knxServerUrl,
                                   WBMQTT::TLogger& errorLogger,
                                   WBMQTT::TLogger& debugLogger,
                                   WBMQTT::TLogger& infoLogger);

        void Send(const TTelegram& telegram) override;
        void SetOnReceive(const std::function<void(const TTelegram&)>& handler) override;

        void Start();
        void Stop();

        ~TKnxClientService() override = default;

    private:
        void Loop();
        void HandleLoopError(const std::string& what);

        std::string KnxServerUrl;
        std::function<void(const TTelegram&)> OnReceiveTelegramHandler;

        bool IsStarted = false;
        std::mutex IsStartedMutex;
        std::mutex SetterMutex;
        std::unique_ptr<std::thread> Worker;

        WBMQTT::TLogger& ErrorLogger;
        WBMQTT::TLogger& DebugLogger;
        WBMQTT::TLogger& InfoLogger;
    };
} // namespace knx

#endif // WB_MQTT_KNX_KNXCLIENTSERVICE_H
