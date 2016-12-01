#include "knx-client.h"
#include "observer.h"
#include <eibclient.h>
#include <eibloadresult.h>
#include <eibtypes.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <wbmqtt/mqtt_wrapper.h>

int main(int argc, char** argv)
{
    TMQTTClient::TConfig mqttConfig;
    std::string knxUrl = "ip:localhost:6720";
    mqttConfig.Host = "localhost";
    mqttConfig.Port = 1883;
    bool debug = false;
    int c;
    while ((c = getopt(argc, argv, "dp:h:k:")) != -1) {
        switch (c) {
        case 'd':
            debug = true;
            break;
        case 'p':
            mqttConfig.Port = std::stoi(optarg);
            break;
        case 'h':
            mqttConfig.Host = optarg;
            break;
        case 'k':
            knxUrl = optarg;
        default:
            break;
        }
    }
    mosqpp::lib_init();

    PMQTTClient mqttClient(new TMQTTClient(mqttConfig));
    try {
        PKnxClient knxClient(new TKnxClient(knxUrl));
        knxClient->SetDebug(debug);

        PMqttKnxObserver observer(new TMqttKnxObserver(mqttClient, knxClient));
        observer->SetDebug(debug);

        observer->SetUp();
        mqttClient->StartLoop();
        observer->Loop();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    return 0;
}
