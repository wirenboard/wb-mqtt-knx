#include "knx-client.h"
#include "observer.h"
#include <eibclient.h>
#include <eibloadresult.h>
#include <eibtypes.h>
#include <getopt.h>
#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SyslogAppender.hh>
#include <unistd.h>
#include <wbmqtt/mqtt_wrapper.h>

#include "logging.h"

int main(int argc, char** argv)
{
    TMQTTClient::TConfig mqttConfig;
    std::string knxUrl = "ip:localhost:6720";
    mqttConfig.Host = "localhost";
    mqttConfig.Port = 1883;
    bool debug = false;
    int c;
    int verboseLevel = 0;
    while ((c = getopt(argc, argv, "vdp:H:k:")) != -1) {
        switch (c) {
        case 'v':
            verboseLevel++;
            break;
        case 'd':
            debug = true;
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
    mosqpp::lib_init();

    // set up logging
    log4cpp::Category& logRoot = log4cpp::Category::getRoot();
    log4cpp::PatternLayout* logLayout = new log4cpp::PatternLayout;
    logLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} %p: %m%n");

    if (debug) {
        auto appender = new log4cpp::OstreamAppender("default", &std::cerr);
        appender->setLayout(logLayout);
        logRoot.addAppender(appender);
        logRoot.setPriority(log4cpp::Priority::DEBUG);
    } else {
        long pid = getpid();
        auto appender =
            new log4cpp::SyslogAppender("default", "wb-mqtt-knx[" + std::to_string(pid) + "]");
        appender->setLayout(logLayout);
        logRoot.addAppender(appender);

        auto priority = log4cpp::Priority::WARN;
        if (verboseLevel == 1) priority = log4cpp::Priority::NOTICE;
        if (verboseLevel == 2) priority = log4cpp::Priority::INFO;
        if (verboseLevel >= 3) priority = log4cpp::Priority::DEBUG;

        logRoot.setPriority(priority);
    }

    PMQTTClient mqttClient(new TMQTTClient(mqttConfig));
    try {
        PKnxClient knxClient(new TKnxClient(knxUrl));

        PMqttKnxObserver observer(new TMqttKnxObserver(mqttClient, knxClient));

        observer->SetUp();
        mqttClient->StartLoop();
        observer->Loop();
    } catch (std::exception& e) {
        LOG(FATAL) << e.what();
        exit(1);
    }

    return 0;
}
