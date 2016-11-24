#include <wbmqtt/mqtt_wrapper.h>
#include <memory>
#include "knx-client.h"

#ifndef OBSERVER_H
#define OBSERVER_H
class TMQTTKnxObserver : public IMQTTObserver, public std::enable_shared_from_this<TMQTTKnxObserver> {
public:
    TMQTTKnxObserver(PMQTTClientBase mqtt_client, PKNXClient knx_client);
    void SetUp();
    void OnConnect(int rc);
    void OnMessage(const mosquitto_message *message);
    void OnSubscribe(int mid, int qos_count, const int *granted_qos);

    void LoopOnce();
    void Loop();
private:
    PMQTTClientBase MQTTClient;
    PKNXClient KNXClient;
};

typedef std::shared_ptr<TMQTTKnxObserver> PMQTTKnxObserver;

#endif // OBSERVER_H
