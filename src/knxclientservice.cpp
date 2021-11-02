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

    void TKnxClientService::Send(const TTelegram& telegram) const
    {

        if (!IsStarted)
            return;

        TKnxConnection Out(KnxServerUrl);
        if (!Out)
            wb_throw(TKnxException, "Failed to open Url: " + KnxServerUrl + ". Is knxd running?");

        if (telegram.IsGroupAddressed()) {
            const int32_t openResult = EIBOpen_GroupSocket(Out.GetEIBConnection(), 0);
            if (openResult == EIB_ERROR_RETURN_VALUE)
                wb_throw(TKnxException, "Failed to open GroupSocket");

            auto tpduPayload = telegram.Tpdu().GetRaw();

            const int32_t sendResult = EIBSendGroup(Out.GetEIBConnection(),
                                                    telegram.GetReceiverAddress(),
                                                    static_cast<int32_t>(tpduPayload.size()),
                                                    tpduPayload.data());
            if (sendResult == EIB_ERROR_RETURN_VALUE)
                wb_throw(TKnxException, "failed to send group telegram");

        } else {
            wb_throw(TKnxException, "Sending individual telegrams is not supported by knxd");
        }
    }

    void TKnxClientService::KnxdConnectProcessing()
    {
        // The loop responsible for connecting to the knxd server
        while (IsStarted) {
            TKnxConnection In(KnxServerUrl);

            if (!In) {
                HandleLoopError("Failed to open KnxServerUrl: " + KnxServerUrl + ". Is knxd running?");

                auto tick = std::chrono::microseconds(0);
                while (IsStarted && (tick < CLIENT_RECONNECT_PERIOD)) {
                    std::this_thread::sleep_for(IN_CONNECTION_REQUEST_TICK);
                    tick += IN_CONNECTION_REQUEST_TICK;
                }
                continue;
            }

            InfoLogger.Log() << "KNX connection successful";

            const int32_t eibOpenResult = EIBOpenVBusmonitor(In.GetEIBConnection());
            if (eibOpenResult == EIB_ERROR_RETURN_VALUE) {
                HandleLoopError("failed to open Busmonitor connection");
                continue;
            }

            KnxdReceiveProcessing(In);
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

    void TKnxClientService::KnxdReceiveProcessing(const TKnxConnection& In)
    {
        const int32_t linuxFileDescriptor = EIB_Poll_FD(In.GetEIBConnection());
        if (linuxFileDescriptor == EIB_ERROR_RETURN_VALUE) {
            HandleLoopError("failed to get Poll fd");
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
                HandleLoopError(std::string("select failed: ") + std::strerror(errno));
                break;
            }

            std::vector<uint8_t> telegram(MAX_TELEGRAM_LENGTH, 0);

            const int32_t packetLen =
                EIBGetBusmonitorPacket(In.GetEIBConnection(), static_cast<int32_t>(telegram.size()), telegram.data());
            if (packetLen == EIB_ERROR_RETURN_VALUE) {
                HandleLoopError(std::string("failed to read Busmonitor packet: ") + std::strerror(errno));
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

            try {
                TTelegram knxTelegram(telegram);
                NotifyAllSubscribers(knxTelegram);
            } catch (const TKnxException& e) {
                ErrorLogger.Log() << e.what();
            } catch (const std::exception& e) {
                ErrorLogger.Log() << e.what();
            }
        }
    }

}
