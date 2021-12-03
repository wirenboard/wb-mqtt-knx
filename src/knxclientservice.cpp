#include "knxclientservice.h"
#include "knxconnection.h"
#include "knxexception.h"
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
            wb_throw(TKnxException, "Failed the knxd connection");

        if (telegram.IsGroupAddressed()) {
            auto tpduPayload = telegram.Tpdu().GetRaw();

            const int32_t sendResult = EIBSendGroup(KnxdConnection->GetEIBConnection(),
                                                    telegram.GetReceiverAddress(),
                                                    static_cast<int32_t>(tpduPayload.size()),
                                                    tpduPayload.data());
            if (sendResult == EIB_ERROR_RETURN_VALUE)
                wb_throw(TKnxException, "Failed to send group telegram");

        } else {
            wb_throw(TKnxException, "Sending individual telegrams is not supported by knxd");
        }
    }

    void TKnxClientService::KnxdConnectProcessing()
    {
        // The loop responsible for connecting to the knxd server
        while (IsStarted) {
            KnxdConnection = std::make_unique<TKnxConnection>(KnxServerUrl);

            if (!KnxdConnection->IsConnected()) {
                HandleLoopError("Failed to open KnxServerUrl: " + KnxServerUrl + ". Is knxd running?");

                auto tick = std::chrono::microseconds(0);
                while (IsStarted && (tick < CLIENT_RECONNECT_PERIOD)) {
                    std::this_thread::sleep_for(IN_CONNECTION_REQUEST_TICK);
                    tick += IN_CONNECTION_REQUEST_TICK;
                }
                continue;
            }

            InfoLogger.Log() << "KNX connection successful";

            const int32_t eibOpenResult = EIBOpen_GroupSocket(KnxdConnection->GetEIBConnection(), 0);
            if (eibOpenResult == EIB_ERROR_RETURN_VALUE) {
                HandleLoopError("failed to open EIB GroupSocket connection");
                continue;
            }

            KnxdReceiveProcessing();
        }
    }

    void TKnxClientService::HandleLoopError(const std::string& what)
    {
        ErrorLogger.Log() << "Error in KNX loop: " << what;
    }

    void TKnxClientService::Start()
    {
        bool expectedIsStartedValue = false;

        if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, true)) {
            wb_throw(knx::TKnxException, "Attempt to start already started driver");
        }

        Worker = WBMQTT::MakeThread("KnxClient thread", {[this] { KnxdConnectProcessing(); }});
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
            HandleLoopError("Failed to get Poll fd");
            return;
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
                HandleLoopError(std::string("Select failed: ") + std::strerror(errno));
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
                HandleLoopError(std::string("Failed to get a group TPDU: ") + std::strerror(errno));
                break;
            }

            tpduPayload.resize(packetLen);

            if (DebugLogger.IsEnabled()) {
                std::stringstream ss;
                ss << "KNX Client received a tpduPayload: ";
                ss << packetLen << " ";
                ss << std::hex << std::noshowbase;
                for (int i = 0; i < packetLen; i++) {
                    ss << "0x" << std::setw(2) << std::setfill('0');
                    ss << (unsigned)tpduPayload[i] << " ";
                }
                DebugLogger.Log() << ss.str();
            }

            try {
                TTelegram knxTelegram;
                knxTelegram.SetReceiverAddress(destEibAddress);
                knxTelegram.SetGroupAddressedFlag(true);

                knxTelegram.SetSourceAddress(srcEibAddress);
                knxTelegram.Tpdu().SetRaw(tpduPayload);
                NotifyAllSubscribers(knxTelegram);
            } catch (const TKnxException& e) {
                ErrorLogger.Log() << e.what();
            } catch (const std::exception& e) {
                ErrorLogger.Log() << e.what();
            }
        }
    }

}
