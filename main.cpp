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
    mqttConfig.Host = "localhost";
    mqttConfig.Port = 1883;
    bool debug = false;
    int c;
    while ((c = getopt(argc, argv, "dp:h:")) != -1) {
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
        default:
            break;
        }
    }
    mosqpp::lib_init();

    PMQTTClient mqttClient(new TMQTTClient(mqttConfig));
    PKnxClient knxClient(new TKnxClient());
    knxClient->SetDebug(debug);
    try {
        PMqttKnxObserver observer(new TMqttKnxObserver(mqttClient, knxClient));
        observer->SetDebug(debug);
        observer->SetUp();
        mqttClient->StartLoop();
        observer->Loop();
    } catch (...) {
        // std::cout << "exception found\n";
    }

    return 0;
}
