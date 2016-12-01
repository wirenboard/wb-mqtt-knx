#include "observer.h"
#include <eibclient.h>
#include <unistd.h>
#include <iostream>
#include <wbmqtt/utils.h>
#include <sstream>

extern bool debug;

TMQTTKnxObserver::TMQTTKnxObserver(PMQTTClientBase mqtt_client, PKNXClient knx_client) : MQTTClient(mqtt_client), KNXClient(knx_client){
    //
}

void TMQTTKnxObserver::SetUp() {
    MQTTClient->Observe(shared_from_this());
    KNXClient->Observe(shared_from_this());
    MQTTClient->Connect();
}

void TMQTTKnxObserver::OnConnect(int rc) {
    std::string prefix = "/devices/knx/controls/data";
    MQTTClient->Publish(NULL, prefix + "/meta/type", "data", 0, true);
    MQTTClient->Subscribe(NULL, prefix + "/on");
}

void TMQTTKnxObserver::OnMessage(const mosquitto_message *message) {
    std::string topic = message->topic;
    std::string payload = static_cast<const char *>(message->payload);
    if(debug) std::cout << "KNX observer topic: " << topic << " message: " << payload << "\n";
    KNXClient->Send(payload);
}

void TMQTTKnxObserver::Loop(){
    KNXClient->Loop();
}

void TMQTTKnxObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos) {
    if(debug) std::cout << "KNS subscription succeeded\n";
}

void TMQTTKnxObserver::OnPackage(uint8_t * buf, int len){
    if(debug) std::cout << "KNX package received\n";
    std::string prefix = "/devices/knx/controls/data";
    std::stringstream ss;
    unsigned ACPI = (buf[6] & 0x3) << 2 | (buf[7] >>6);
    int data_len = ((buf[5] & 0xf));
    ss << (buf[1]<<8 | buf[2]) << " " << (buf[3]<<8 | buf[4]) << " " << ACPI << " ";
    if(data_len == 2){
     ss << (buf[7]&0x7f);
    }else {
     buf[data_len+7] = 0;
     ss << (const char *)(buf+8);
    }
    MQTTClient->Publish(NULL, prefix, ss.str());
}
