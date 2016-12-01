#include "observer.h"
#include <eibclient.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <wbmqtt/utils.h>

extern bool debug;

TMqttKnxObserver::TMqttKnxObserver(PMQTTClientBase mqttClient, PKnxClient knxClient)
    : MqttClient(mqttClient), KnxClient(knxClient), Debug(false)
{
}

void TMqttKnxObserver::SetUp()
{
    MqttClient->Observe(shared_from_this());
    KnxClient->Observe(shared_from_this());
    MqttClient->Connect();
}

void TMqttKnxObserver::OnConnect(int rc)
{
    if (Debug) {
        std::cout << "KNX Observer OnConnect handler" << std::endl;
    }

    std::string prefix = "/devices/knx/controls/data";
    MqttClient->Publish(NULL, prefix + "/meta/type", "data", 0, true);
    MqttClient->Subscribe(NULL, prefix + "/on");
}

void TMqttKnxObserver::OnMessage(const mosquitto_message* message)
{
    std::string topic = message->topic;
    std::string payload = static_cast<const char*>(message->payload);
    KnxClient->SendTelegram(payload);
}

void TMqttKnxObserver::Loop()
{
    KnxClient->Loop();
}

void TMqttKnxObserver::OnSubscribe(int mid, int qosCount, const int* grantedQos)
{
}

void TMqttKnxObserver::OnTelegram(uint8_t* buf, int len)
{
    std::string prefix = "/devices/knx/controls/data";
    std::stringstream ss;
    unsigned acpi = (buf[6] & 0x3) << 2 | (buf[7] >> 6);
    int dataLen = ((buf[5] & 0xf));
    uint16_t srcAddr = (buf[1] << 8 | buf[2]);
    uint16_t dstAddr = (buf[3] << 8 | buf[4]);
    ss << (srcAddr >> 12) << "/" << ((srcAddr >> 8) & 0xf) << "/" << (srcAddr & 0xff) << " ";
    if (buf[5] >> 7) {
        // group address
        ss << ((dstAddr >> 11) & 0xf) << "/" << ((dstAddr >> 8) & 0x7) << "/" << (dstAddr & 0xff);
    } else {
        // individual address
        ss << (dstAddr >> 12) << "/" << ((dstAddr >> 8) & 0xf) << "/" << (dstAddr & 0xff);
    }
    ss << " " << acpi << " ";
    if (dataLen == 2) {
        ss << (buf[7] & 0x7f);
    } else {
        buf[dataLen + 7] = 0;
        ss << (const char*)(buf + 8);
    }
    MqttClient->Publish(NULL, prefix, ss.str());
}

bool TMqttKnxObserver::SetDebug(bool debug)
{
    bool ret = Debug;
    Debug = debug;
    return ret;
}
