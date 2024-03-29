#include "../../src/knxgroupobject/dptcommonconfig.h"
#include "../../src/knxgroupobject/dptjsonconfig.h"
#include "../../src/knxgroupobject/dptwbmqttconfig.h"
#include "config.h"
#include "etsconfigtool.h"
#include <getopt.h>
#include <iostream>

namespace
{
    struct TAppOptions
    {
        std::string EtsExportFilePath;
        std::string WbMqttKnxConfigPath;
    };

    void PrintUsage(const std::string& appName)
    {
        std::cout << "Usage:" << std::endl
                  << " " << appName << " [options] ETS_CONFIG WB_MQTT_KNX_CONFIG" << std::endl
                  << "Options:" << std::endl
                  << "  -h  help" << std::endl
                  << std::endl;
    }

    void ParseCommandLine(int argc, char* argv[], TAppOptions& options)
    {
        int c;

        while ((c = getopt(argc, argv, "h")) != -1) {
            switch (c) {
                case '?':
                case 'h':
                    PrintUsage(argv[0]);
                    exit(0);
                default:
                    PrintUsage(argv[0]);
                    exit(2);
            }
        }

        if (optind + 2 == argc) {
            options.EtsExportFilePath = argv[optind];
            options.WbMqttKnxConfigPath = argv[optind + 1];
        } else {
            PrintUsage(argv[0]);
            exit(2);
        }
    }
}

int main(int argc, char** argv)
{
    TAppOptions appOptions;
    ParseCommandLine(argc, argv, appOptions);

    knx::object::TDptWbMqttConfig mqttConfig;
    knx::object::TDptJsonConfig jsonConfig(DEFAULT_CONFIG_JSON_DATAPOINT_FILE_PATH);
    knx::tool::TEtsConfigTool converter(mqttConfig, jsonConfig, knx::object::dptcommonconfig::RawValueId);

    try {
        converter.LoadEtsExport(appOptions.EtsExportFilePath);
        converter.SaveWbMqttConfig(appOptions.WbMqttKnxConfigPath);
    } catch (const std::exception& exception) {
        std::cerr << exception.what();
        exit(1);
    }
    return 0;
}