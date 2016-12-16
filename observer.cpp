#include "observer.h"
#include "Telegram.h"
#include <eibclient.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <wbmqtt/utils.h>

#include "logging.h"
extern bool debug;

TMqttKnxObserver::TMqttKnxObserver(PMQTTClientBase mqttClient, PKnxClient knxClient)
    : MqttClient(mqttClient), KnxClient(knxClient)
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
    LOG(INFO) << "Observer OnConnect handler";

    std::string device = "/devices/knx";
    std::string prefix = device + "/controls/data";

    MqttClient->Publish(NULL, prefix + "/meta/type", "data", 0, true);
    MqttClient->Publish(NULL, device + "/meta/name", "wb-knx", 0, true);

    MqttClient->Subscribe(NULL, prefix + "/on");
}

void TMqttKnxObserver::OnMessage(const mosquitto_message* message)
{
    std::string payload = static_cast<const char*>(message->payload);
    LOG(INFO) << "Mosquitto message: " << payload;
    try {
        KnxClient->SendTelegram(payload);
    } catch (TKnxException& e) {
        LOG(ERROR) << "Failed to send telegram: " << e.what();
    }
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
    try {
        TKnxTelegram t(buf, len);
        std::string prefix = "/devices/knx/controls/data";
        MqttClient->Publish(NULL, prefix, t.GetMqttPayload());
    } catch (TKnxException& e) {
        LOG(ERROR) << "Failed to parse telegram: " << e.what();
    }
}
