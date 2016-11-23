#include <iostream>
#include <wbmqtt/mqtt_wrapper.h>
#include <getopt.h>
#include <unistd.h>
#include <eibclient.h>
#include <eibloadresult.h>
#include <eibtypes.h>
#include "observer.h"

bool debug;

int main(int argc, char ** argv)
{
    debug = true;
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Host = "localhost";
    mqtt_config.Port = 1883;
    int c;
    while ((c = getopt(argc, argv, "dp:h:"))!=-1) {
        switch (c){
        case 'd':
            debug = true;
            break;
        case 'p':
            mqtt_config.Port = std::stoi(optarg);
            break;
        case 'h':
            mqtt_config.Host = optarg;
            break;
        default:
            break;
        }
    }
    mosqpp::lib_init();
    mqtt_config.Id = "wb-knx";

    PMQTTClient mqtt_client(new TMQTTClient(mqtt_config));
    try {
        PMQTTKnxObserver observer(new TMQTTKnxObserver(mqtt_client));
        observer->SetUp();
        mqtt_client->StartLoop();
        observer->Loop();
    } catch(...){
        std::cout << "exception found\n";
    }

    return 0;
}

