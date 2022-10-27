#include "knxclientservice.h"
#include "knxconnection.h"
#include "knxexception.h"
#include "knxgroupaddress.h"
#include "knxindividualaddress.h"
#include "wblib/utils.h"
#include <cstring>
#include <eibclient.h>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <utility>

namespace
{
    constexpr auto CLIENT_RECONNECT_PERIOD = std::chrono::seconds(30);
    constexpr auto RECEIVER_LOOP_TIMEOUT = std::chrono::seconds(2);
    constexpr auto IN_CONNECTION_REQUEST_TICK = std::chrono::seconds(1);

    constexpr auto MAX_TELEGRAM_LENGTH = knx::TTelegram::SizeWithoutPayload + knx::TTpdu::MaxPayloadSize;
    constexpr auto EIB_ERROR_RETURN_VALUE = -1;
    constexpr auto SELECT_TIMEOUT_RETURN_VALUE = 0;
    constexpr auto SELECT_ERROR_RETURN_VALUE = -1;

    enum class LogType
    {
        RECEIVE,
        SEND
    };

    std::string ToLog(const knx::TTelegram& telegram, LogType logType)
    {
        auto tpdu = telegram.Tpdu().GetRaw();
        std::stringstream ss;
        if (logType == LogType::RECEIVE) {
            ss << "from:" << knx::TKnxIndividualAddress(telegram.GetSourceAddress()).ToString() << " ";
        }
        ss << "to:" << knx::TKnxGroupAddress(telegram.GetReceiverAddress()).ToString() << " ";
        ss << "tpdu(";
        ss << tpdu.size() << "):";
        ss << std::hex << std::noshowbase;
        for (unsigned char i: tpdu) {
            ss << std::setw(2) << std::setfill('0');
            ss << (unsigned)i << " ";
        }
        return ss.str();
    }

} // namespace

namespace knx
{
    TKnxClientService::TKnxClientService(const std::string& knxServerUrl,
                                         WBMQTT::TLogger& errorLogger,
                                         WBMQTT::TLogger& debugLogger,
                                         WBMQTT::TLogger& infoLogger)
        : KnxServerUrl{knxServerUrl},
          ErrorLogger(errorLogger),
          DebugLogger(debugLogger),
          InfoLogger(infoLogger)
    {}

    void TKnxClientService::Send(const TTelegram& telegram)
    {
        if (!IsStarted)
            return;

        if (!KnxdConnection->IsConnected())
            HandleCriticalError("Failed the knxd connection");

        if (telegram.IsGroupAddressed()) {
            auto tpduPayload = telegram.Tpdu().GetRaw();
            int32_t sendResult;
            {
                std::lock_guard<std::mutex> lg(SendMutex);
                sendResult = EIBSendGroup(KnxdConnection->GetEIBConnection(),
                                          telegram.GetReceiverAddress(),
                                          static_cast<int32_t>(tpduPayload.size()),
                                          tpduPayload.data());
            }
            if (sendResult != EIB_ERROR_RETURN_VALUE) {
                if (DebugLogger.IsEnabled()) {
                    DebugLogger.Log() << "Sent to knxd: " << ToLog(telegram, LogType::SEND);
                }
            } else {
                ErrorLogger.Log() << "Failed to send group telegram";
            }

        } else {
            ErrorLogger.Log() << "Sending individual telegrams is not supported by knxd";
        }
    }

    void TKnxClientService::KnxdConnectProcessing()
    {
        InfoLogger.Log() << "Connecting to knxd... URL: '" << KnxServerUrl << "'";
        KnxdConnection = std::make_unique<TKnxConnection>(KnxServerUrl);

        if (!KnxdConnection->IsConnected()) {
            HandleCriticalError("Failed to open KnxServerUrl: " + KnxServerUrl + ". Is knxd running?");
        }

        const int32_t eibOpenResult = EIBOpen_GroupSocket(KnxdConnection->GetEIBConnection(), 0);
        if (eibOpenResult == EIB_ERROR_RETURN_VALUE) {
            HandleCriticalError("failed to open EIB GroupSocket connection");
        }

        InfoLogger.Log() << "knxd connected successfully";
    }

    void TKnxClientService::HandleCriticalError(const std::string& what)
    {
        ErrorLogger.Log() << what;
        wb_throw(knx::TKnxException, what);
    }

    void TKnxClientService::Start()
    {
        bool expectedIsStartedValue = false;

        if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, true)) {
            wb_throw(knx::TKnxException, "Attempt to start already started driver");
        }

        KnxdConnectProcessing();

        Worker = WBMQTT::MakeThread("KnxClient thread", {[this] { KnxdReceiveProcessing(); }});
        NotifyAllSubscribers(TKnxEvent::KnxdSocketConnected, TTelegram{});
    }

    void TKnxClientService::Stop()
    {
        bool expectedIsStartedValue = true;

        if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, false)) {
            wb_throw(knx::TKnxException, "Attempt to stop already stopped TKnxClientService");
        }

        if (Worker->joinable()) {
            Worker->join();
        }
        Worker.reset();
    }

    void TKnxClientService::KnxdReceiveProcessing()
    {
        const int32_t linuxFileDescriptor = EIB_Poll_FD(KnxdConnection->GetEIBConnection());
        if (linuxFileDescriptor == EIB_ERROR_RETURN_VALUE) {
            HandleCriticalError("Failed to get Poll fd");
        }
        // The loop responsible for receiving telegrams from knxd
        while (IsStarted) {
            struct timeval tv = {0,
                                 std::chrono::duration_cast<std::chrono::microseconds>(RECEIVER_LOOP_TIMEOUT).count()};
            fd_set set;
            FD_ZERO(&set);
            FD_SET(linuxFileDescriptor, &set);

            const int32_t selectResult = select(linuxFileDescriptor + 1, &set, nullptr, nullptr, &tv);
            if (selectResult == SELECT_TIMEOUT_RETURN_VALUE) {
                continue;
            } else if (selectResult == SELECT_ERROR_RETURN_VALUE) {
                HandleCriticalError(std::string("Select failed: ") + std::strerror(errno));
                break;
            }

            std::vector<uint8_t> tpduPayload(MAX_TELEGRAM_LENGTH, 0);

            eibaddr_t srcEibAddress;
            eibaddr_t destEibAddress;

            const int32_t packetLen = EIBGetGroup_Src(KnxdConnection->GetEIBConnection(),
                                                      static_cast<int32_t>(tpduPayload.size()),
                                                      tpduPayload.data(),
                                                      &srcEibAddress,
                                                      &destEibAddress);
            if (packetLen == EIB_ERROR_RETURN_VALUE) {
                NotifyAllSubscribers(TKnxEvent::KnxdSocketError, TTelegram{});
                HandleCriticalError(std::string("Failed to get a group TPDU: ") + std::strerror(errno));
            }

            tpduPayload.resize(packetLen);

            try {
                TTelegram knxTelegram;
                knxTelegram.SetReceiverAddress(destEibAddress);
                knxTelegram.SetGroupAddressedFlag(true);

                knxTelegram.SetSourceAddress(srcEibAddress);
                knxTelegram.Tpdu().SetRaw(tpduPayload);

                if (DebugLogger.IsEnabled()) {
                    DebugLogger.Log() << "Received from knxd: " << ToLog(knxTelegram, LogType::RECEIVE);
                }
                NotifyAllSubscribers(TKnxEvent::ReceivedTelegram, knxTelegram);
            } catch (const TKnxException& e) {
                ErrorLogger.Log() << e.what();
            } catch (const std::exception& e) {
                ErrorLogger.Log() << e.what();
            }
        }
    }

}
