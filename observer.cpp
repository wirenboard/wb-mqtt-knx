#include "observer.h"
#include <eibclient.h>
#include <unistd.h>
#include <iostream>
#include <wbmqtt/utils.h>

extern bool debug;

TMQTTKnxObserver::TMQTTKnxObserver(PMQTTClientBase mqtt_client) : MQTTClient(mqtt_client) {
    //
}
//mqtt
void TMQTTKnxObserver::SetUp() {
    MQTTClient->Observe(shared_from_this());
    MQTTClient->Connect();
}

void TMQTTKnxObserver::OnConnect(int rc) {
    if(debug) std::cout << "KNX observer on connect\n";
    MQTTClient->Subscribe(NULL, "/devices/knx/#");
}

void TMQTTKnxObserver::OnMessage(const mosquitto_message *message) {
    std::string topic = message->topic;
    std::string payload = static_cast<const char *>(message->payload);
    if(debug) std::cout << "KNX observer topic: " << topic << " message: " << payload << "\n";
    KNXClient->Send(topic, payload);
}
//knx
void TMQTTKnxObserver::Loop(){
    // TODO run main cycle of KNX client
    KNXClient->Loop();
}
void TMQTTKnxObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos) {
    if(debug) std::cout << "KNS subscription succeeded\n";
}
