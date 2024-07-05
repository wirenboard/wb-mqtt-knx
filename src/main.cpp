#include "config.h"
#include "configurator.h"
#include "knxclientservice.h"
#include "knxgroupobject/dptjsonbuilder.h"
#include "knxgroupobject/dptwbmqttbuilder.h"
#include "knxgroupobject/mqttbuilder.h"
#include "knxgroupobjectcontroller.h"
#include "knxlegacydevice.h"
#include "ticktimer.h"
#include <getopt.h>
#include <unistd.h>
#include <wblib/log.h>
#include <wblib/signal_handling.h>
#include <wblib/wbmqtt.h>

namespace
{
    const auto DEFAULT_KNX_URL = "local:/var/run/knx";

    const auto KNX_DRIVER_STOP_TIMEOUT_S = std::chrono::seconds(60); // topic cleanup can take a lot of time
    const auto KNX_READ_TICK_PERIOD = std::chrono::milliseconds(50);

    // https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/iniscrptact.html
    constexpr auto EXIT_NOTCONFIGURED = 6; // The program is not configured

    WBMQTT::TLogger ErrorLogger("ERROR: ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::RED);
    WBMQTT::TLogger DebugLogger("DEBUG: ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::WHITE, false);
    WBMQTT::TLogger InfoLogger("INFO: ", WBMQTT::TLogger::StdErr, WBMQTT::TLogger::GREY);

    void PrintUsage()
    {
        std::cout << "Usage: " << PROJECT_NAME << " [options]" << std::endl
                  << "Options:" << std::endl
                  << "  -d       level     enable debuging output:" << std::endl
                  << "                       1 - knx only;" << std::endl
                  << "                       2 - mqtt only;" << std::endl
                  << "                       3 - both;" << std::endl
                  << "                       negative values - silent mode (-1, -2, -3))" << std::endl
                  << "  -c       config    config file (default: " << DEFAULT_CONFIG_FILE_PATH << ")" << std::endl
                  << "  -p       port      MQTT broker port (default: 1883)" << std::endl
                  << "  -H       IP        MQTT broker IP (default: localhost)" << std::endl
                  << "  -u       user      MQTT user (optional)" << std::endl
                  << "  -P       password  MQTT user password (optional)" << std::endl
                  << "  -T       prefix    MQTT topic prefix (optional)" << std::endl
                  << "  -k       url       KNX url (default: " << DEFAULT_KNX_URL << ")" << std::endl;
    }

    void SetDebugLevel(const char* optarg)
    {
        try {
            auto debugLevel = std::stoi(optarg);
            switch (debugLevel) {
                case 0:
                    return;
                case -1:
                    InfoLogger.SetEnabled(false);
                    return;
                case -2:
                    WBMQTT::Info.SetEnabled(false);
                    return;
                case -3:
                    WBMQTT::Info.SetEnabled(false);
                    InfoLogger.SetEnabled(false);
                    return;
                case 1:
                    DebugLogger.SetEnabled(true);
                    return;
                case 2:
                    WBMQTT::Debug.SetEnabled(true);
                    return;
                case 3:
                    WBMQTT::Debug.SetEnabled(true);
                    DebugLogger.SetEnabled(true);
                    return;
            }
        } catch (...) {
        }
        std::cout << "Invalid -d parameter value " << optarg << std::endl;
        PrintUsage();
        exit(2);
    }

    void ParseCommadLine(int argc,
                         char* argv[],
                         WBMQTT::TMosquittoMqttConfig& mqttConfig,
                         std::string& customConfig,
                         std::string& knxUrl)
    {
        int c;
        while ((c = getopt(argc, argv, "d:c:p:H:u:P:T:k:")) != -1) {
            switch (c) {
                case 'd':
                    SetDebugLevel(optarg);
                    break;
                case 'c':
                    customConfig = optarg;
                    break;
                case 'p':
                    mqttConfig.Port = std::stoi(optarg);
                    break;
                case 'H':
                    mqttConfig.Host = optarg;
                    break;
                case 'T':
                    mqttConfig.Prefix = optarg;
                    break;
                case 'u':
                    mqttConfig.User = optarg;
                    break;
                case 'P':
                    mqttConfig.Password = optarg;
                    break;
                case 'k':
                    knxUrl = optarg;
                    break;
                default:
                    PrintUsage();
                    exit(2); // EXIT_INVALIDARGUMENT
            }
        }

        if (optind < argc) {
            for (int index = optind; index < argc; ++index) {
                std::cout << "Skipping unknown argument " << argv[index] << std::endl;
            }
        }
    }
} // namespace

int main(int argc, char** argv)
{
    WBMQTT::TMosquittoMqttConfig mqttConfig;
    std::string configFilename(DEFAULT_CONFIG_FILE_PATH);
    std::string knxUrl(DEFAULT_KNX_URL);
    mqttConfig.Id = PROJECT_NAME;

    ParseCommadLine(argc, argv, mqttConfig, configFilename, knxUrl);

    WBMQTT::SetThreadName(PROJECT_NAME);
    WBMQTT::SignalHandling::Handle({SIGINT, SIGTERM});
    WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
        WBMQTT::SignalHandling::Stop();
        InfoLogger.Log() << "wb-mqtt-knx service stopped";
    });

    /* if handling of signal takes too much time: exit with error */
    WBMQTT::SignalHandling::SetOnTimeout(KNX_DRIVER_STOP_TIMEOUT_S, [&] {
        ErrorLogger.Log() << "Driver takes too long to stop. Exiting.";
        exit(EXIT_FAILURE);
    });

    std::unique_ptr<knx::Configurator> pConfigurator;

    try {
        pConfigurator = std::make_unique<knx::Configurator>(configFilename, DEFAULT_CONFIG_SCHEMA_FILE_PATH);
    } catch (std::exception& e) {
        ErrorLogger.Log() << e.what();
        return EXIT_NOTCONFIGURED;
    }

    try {
        if (pConfigurator->IsDebugEnabled()) {
            DebugLogger.SetEnabled(true);
        }

        auto mqttClient = WBMQTT::NewMosquittoMqttClient(mqttConfig);
        auto mqttDriver = WBMQTT::NewDriver(WBMQTT::TDriverArgs{}
                                                .SetId(PROJECT_NAME)
                                                .SetBackend(WBMQTT::NewDriverBackend(mqttClient))
                                                .SetUseStorage(false)
                                                .SetReownUnknownDevices(true));
        mqttDriver->StartLoop();

        WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
            mqttDriver->StopLoop();
            mqttDriver->Close();
        });

        mqttDriver->WaitForReady();

        auto knxClientService = std::make_shared<knx::TKnxClientService>(knxUrl, ErrorLogger, DebugLogger, InfoLogger);

        std::shared_ptr<knx::TKnxLegacyDevice> knxLegacyDevice;
        if (pConfigurator->IsKnxLegacyDeviceEnabled()) {
            knxLegacyDevice = std::make_shared<knx::TKnxLegacyDevice>(mqttDriver,
                                                                      knxClientService,
                                                                      ErrorLogger,
                                                                      DebugLogger,
                                                                      InfoLogger);
        }
        knx::TTickTimer tickTimer;
        tickTimer.SetTickPeriod(KNX_READ_TICK_PERIOD);
        auto knxGroupObjectController =
            std::make_shared<knx::TKnxGroupObjectController>(knxClientService, KNX_READ_TICK_PERIOD);

        knx::object::TDptJsonBuilder dptJsonBuilder(DEFAULT_CONFIG_JSON_DATAPOINT_FILE_PATH);
        knx::object::TDptWbMqttBuilder dptWbMqttBuilder;
        auto groupObjectBuilder = std::make_shared<knx::object::TGroupObjectMqttBuilder>(mqttDriver,
                                                                                         dptJsonBuilder,
                                                                                         dptWbMqttBuilder,
                                                                                         ErrorLogger);

        WBMQTT::SignalHandling::OnSignals({SIGINT, SIGTERM}, [&] {
            tickTimer.Stop();
            tickTimer.Unsubscribe(knxGroupObjectController);

            knxClientService->Unsubscribe(knxGroupObjectController);
            groupObjectBuilder->Clear();

            if (knxLegacyDevice) {
                knxClientService->Unsubscribe(knxLegacyDevice);
                knxLegacyDevice->Deinit();
            }

            knxClientService->Stop();
        });

        if (knxLegacyDevice) {
            knxClientService->Subscribe(knxLegacyDevice);
        }
        knxClientService->Subscribe(knxGroupObjectController);
        tickTimer.Subscribe(knxGroupObjectController);

        pConfigurator->ConfigureObjectController(*knxGroupObjectController, *groupObjectBuilder);

        knxClientService->Start();
        tickTimer.Start();

        WBMQTT::SignalHandling::Start();
        WBMQTT::SignalHandling::Wait();
    } catch (std::exception& e) {
        ErrorLogger.Log() << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
