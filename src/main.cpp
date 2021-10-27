#include "knxclientservice.h"
#include "knxdevice.h"
#include <getopt.h>
#include <unistd.h>
#include <wblib/log.h>
#include <wblib/signal_handling.h>
#include <wblib/wbmqtt.h>

namespace
{
    constexpr auto DRIVER_ID = "wb-mqtt-knx";
    const auto KNX_DRIVER_INIT_TIMEOUT_S = std::chrono::seconds(30);
    const auto KNX_DRIVER_STOP_TIMEOUT_S = std::chrono::seconds(60); // topic cleanup can take a lot of time

    WBMQTT::TLogger ErrorLogger("ERROR: [knx] ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::RED);
    WBMQTT::TLogger VerboseLogger("INFO: [knx] ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::WHITE, false);
    WBMQTT::TLogger InfoLogger("INFO: [knx] ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::GREY);
} // namespace

int main(int argc, char** argv)
{
    WBMQTT::TMosquittoMqttConfig mqttConfig;
    std::string knxUrl = "ip:localhost:6720";
    mqttConfig.Host = "localhost";
    mqttConfig.Port = 1883;
    mqttConfig.Id = "wb-knx";

    int c;
    int verboseLevel = 0;
    while ((c = getopt(argc, argv, "vdp:H:k:")) != -1) {
        switch (c) {
            case 'v':
                verboseLevel++;
                break;
            case 'd':
                break;
            case 'p':
                mqttConfig.Port = std::stoi(optarg);
                break;
            case 'H':
                mqttConfig.Host = optarg;
                break;
            case 'k':
                knxUrl = optarg;
                break;
            default:
                break;
        }
    }

#ifdef NDEBUG
    if (verboseLevel > 0) {
        VerboseLogger.SetEnabled(true);
    }
#else
    VerboseLogger.SetEnabled(true);
#endif

    WBMQTT::TPromise<void> initialized;

    WBMQTT::SetThreadName("wb-mqtt-knx");
    WBMQTT::SignalHandling::Handle({SIGINT, SIGTERM, SIGHUP});
    WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
        WBMQTT::SignalHandling::Stop();
        InfoLogger.Log() << "wb-mqtt-knx service stopped";
    });

    /* if signal arrived before driver is initialized:
    wait some time to initialize and then exit gracefully
    else if timed out: exit with error
*/
    WBMQTT::SignalHandling::SetWaitFor(KNX_DRIVER_INIT_TIMEOUT_S, initialized.GetFuture(), [&] {
        ErrorLogger.Log() << "Driver takes too long to initialize. Exiting.";
        exit(1);
    });

    /* if handling of signal takes too much time: exit with error */
    WBMQTT::SignalHandling::SetOnTimeout(KNX_DRIVER_STOP_TIMEOUT_S, [&] {
        ErrorLogger.Log() << "Driver takes too long to stop. Exiting.";
        exit(2);
    });
    WBMQTT::SignalHandling::Start();

    try {

        auto mqttClient = WBMQTT::NewMosquittoMqttClient(mqttConfig);
        auto mqttDriver = WBMQTT::NewDriver(WBMQTT::TDriverArgs{}
                                                .SetId(DRIVER_ID)
                                                .SetBackend(WBMQTT::NewDriverBackend(mqttClient))
                                                .SetUseStorage(false)
                                                .SetReownUnknownDevices(true));
        mqttDriver->StartLoop();

        WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
            mqttDriver->StopLoop();
            mqttDriver->Close();
        });

        mqttDriver->WaitForReady();

        auto knxClientService =
            std::make_shared<knx::TKnxClientService>(knxUrl, ErrorLogger, VerboseLogger, InfoLogger);
        auto knxDevice =
            std::make_shared<knx::TKnxDevice>(mqttDriver, knxClientService, ErrorLogger, VerboseLogger, InfoLogger);

        WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
            knxClientService->Stop();
            knxDevice->Deinit();
        });

        knxClientService->Start();

        initialized.Complete();
        WBMQTT::SignalHandling::Wait();
    } catch (std::exception& e) {
        ErrorLogger.Log() << e.what();
        WBMQTT::SignalHandling::Stop();
        exit(1);
    }

    return 0;
}
