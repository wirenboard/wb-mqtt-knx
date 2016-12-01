#include "knx-client.h"
#include <memory>
#include <wbmqtt/mqtt_wrapper.h>

#ifndef OBSERVER_H
#define OBSERVER_H

class TMqttKnxObserver : public IMQTTObserver,
                         public std::enable_shared_from_this<TMqttKnxObserver> {
public:
    TMqttKnxObserver(PMQTTClientBase mqttClient, PKnxClient knxClient);
    void SetUp();
    void OnConnect(int rc);
    void OnMessage(const mosquitto_message* message);
    void OnSubscribe(int mid, int qosCount, const int* grantedQos);
    bool SetDebug(bool debug);

    void OnTelegram(uint8_t* package, int len);

    void LoopOnce();
    void Loop();

private:
    PMQTTClientBase MqttClient;
    PKnxClient KnxClient;
    bool Debug;
};

typedef std::shared_ptr<TMqttKnxObserver> PMqttKnxObserver;

#endif // OBSERVER_H
