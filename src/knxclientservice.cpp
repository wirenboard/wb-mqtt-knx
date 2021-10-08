#include "knxclientservice.h"
#include "knxexception.h"
#include "wblib/utils.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <utility>

namespace
{
    constexpr auto MAX_TELEGRAM_LENGTH = knx::TTelegram::SizeWithoutPayload + knx::TTelegram::MaxPayloadSize;
    constexpr auto CLIENT_RECONNECT_PERIOD_S = 30;
    constexpr auto RECEIVER_LOOP_TIMEOUT_S   = 2;
} // namespace

namespace knx
{

    TKnxClientService::TKnxClientService(std::string      knxServerUrl,
                                         WBMQTT::TLogger& errorLogger,
                                         WBMQTT::TLogger& debugLogger,
                                         WBMQTT::TLogger& infoLogger)
        : KnxServerUrl{std::move(knxServerUrl)}, ErrorLogger(errorLogger), DebugLogger(debugLogger),
          InfoLogger(infoLogger)
    {
    }

    void TKnxClientService::Send(const TTelegram& telegram)
    {
        {
            std::lock_guard<std::mutex> lg(IsStartedMutex);
            if (!IsStarted) return;
        }
        TKnxConnection Out(KnxServerUrl);
        if (!Out)
            wb_throw(TKnxException, "Failed to open Url: " + KnxServerUrl + ". Is knxd running?");

        int res;
        if (telegram.IsGroupAddressed()) {
            res = EIBOpen_GroupSocket(Out.GetEIBConnection(), 0);
            if (res == -1)
                wb_throw(TKnxException, "Failed to open GroupSocket");

            auto tpduPayload = telegram.GetTPDUPayload();

            res = EIBSendGroup(Out.GetEIBConnection(),
                               telegram.GetReceiverAddress(),
                               static_cast<int32_t>(tpduPayload.size()),
                               tpduPayload.data());
            if (res == -1)
                wb_throw(TKnxException, "failed to send group telegram");

        } else {
            wb_throw(TKnxException, "Sending individual telegrams is not supported by knxd");
        }
    }

    void TKnxClientService::Loop()
    {
        while (IsStarted) {
            TKnxConnection In(KnxServerUrl);

            if (!In) {
                HandleLoopError("Failed to open KnxServerUrl: " + KnxServerUrl + ". Is knxd running?",
                                CLIENT_RECONNECT_PERIOD_S);
                continue;
            }

            InfoLogger.Log() << "KNX connection successful";

            const int32_t eibOpenResult = EIBOpenVBusmonitor(In.GetEIBConnection());
            if (eibOpenResult == -1) {
                HandleLoopError("failed to open Busmonitor connection", 0);
                continue;
            }

            const int32_t fd = EIB_Poll_FD(In.GetEIBConnection());
            if (fd == -1) {
                HandleLoopError("failed to get Poll fd", 0);
                continue;
            }

            while (IsStarted) {
                struct timeval tv = {RECEIVER_LOOP_TIMEOUT_S, 0};
                fd_set         set;
                FD_ZERO(&set);
                FD_SET(fd, &set);

                const int32_t selectResult = select(fd + 1, &set, nullptr, nullptr, &tv);
                if (selectResult == 0) {
                    continue;
                } else if (selectResult == -1) {
                    HandleLoopError(std::string("select failed: ") + std::strerror(errno), 0);
                    break;
                }

                std::vector<uint8_t> telegram(MAX_TELEGRAM_LENGTH, 0);

                const int32_t packetLen = EIBGetBusmonitorPacket(In.GetEIBConnection(),
                                                                 static_cast<int32_t>(telegram.size()),
                                                                 telegram.data());
                if (packetLen == -1) {
                    HandleLoopError(std::string("failed to read Busmonitor packet: ") + std::strerror(errno),
                                    0);
                    break;
                }

                telegram.resize(packetLen);

                if (DebugLogger.IsEnabled()) {
                    std::stringstream ss;
                    ss << "KNX Client received a telegram: ";
                    ss << packetLen << " ";
                    ss << std::hex << std::noshowbase;
                    for (int i = 0; i < packetLen; i++) {
                        ss << "0x" << std::setw(2) << std::setfill('0');
                        ss << (unsigned)telegram[i] << " ";
                    }
                    DebugLogger.Log() << ss.str();
                }

                if (OnReceiveTelegramHandler)
                    OnReceiveTelegramHandler(TTelegram(telegram));
            }
        }
    }

    void TKnxClientService::HandleLoopError(const std::string& what, unsigned int timeout)
    {
        ErrorLogger.Log() << "Error in KNX loop: " << what;
        sleep(timeout);
    }

    void TKnxClientService::SetOnReceive(const std::function<void(const TTelegram&)>& handler)
    {
        std::lock_guard<std::mutex> lg(SetterMutex);
        OnReceiveTelegramHandler = handler;
    }

    void TKnxClientService::Start()
    {
        {
            std::lock_guard<std::mutex> lg(IsStartedMutex);
            if (IsStarted) {
                wb_throw(knx::TKnxException, "Attempt to start already started driver");
            }
            IsStarted = true;
        }

        Worker = WBMQTT::MakeThread("KnxClient thread", {[this] { Loop(); }});
    }

    void TKnxClientService::Stop()
    {
        {
            std::lock_guard<std::mutex> lg(IsStartedMutex);
            if (!IsStarted) {
                wb_throw(knx::TKnxException, "Attempt to stop already stopped TKnxDevice");
            }
            IsStarted = false;
        }
        OnReceiveTelegramHandler = {};
        if (Worker->joinable()) {
            Worker->join();
        }
        Worker.reset();
    }

} // namespace knx