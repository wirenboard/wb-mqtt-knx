#pragma once

#include "isender.h"
#include "knxconnection.h"
#include "knxevent.h"
#include "knxtelegram.h"
#include "observer.h"
#include "wblib/log.h"
#include <list>

namespace knx
{
    /// \brief Class for transmitting and receiving KNX telegrams
    class TKnxClientService: public ISender<TTelegram>,
                             public TObserver<TKnxEvent, TTelegram>
    {
    public:
        /// Constructor
        /// \param knxServerUrl the knxd server URL
        /// \param errorLogger a Error logger object
        /// \param debugLogger a Debug logger object
        /// \param infoLogger a Info logger object
        explicit TKnxClientService(const std::string& knxServerUrl,
                                   WBMQTT::TLogger& errorLogger,
                                   WBMQTT::TLogger& debugLogger,
                                   WBMQTT::TLogger& infoLogger);

        /// Send KNX telegram
        /// \param telegram a KNX telegram
        void Send(const TTelegram& telegram) override;

        /// Start service
        void Start();

        /// Strop service
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
