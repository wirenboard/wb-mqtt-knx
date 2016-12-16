#include "observer.h"
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
    std::vector<std::string> names = {"GroupValueRead",
                                      "GroupValueResponse",
                                      "GroupValueWrite",
                                      "IndividualAddrWrite",
                                      "IndividualAddrRequest",
                                      "IndividualAddrResponse",
                                      "AdcRead",
                                      "AdcResponse",
                                      "MemoryRead",
                                      "MemoryRead",
                                      "MemoryWrite",
                                      "UserMessage",
                                      "MaskVersionRead",
                                      "MaskVersionResponse",
                                      "Restart",
                                      "Escape"};
    if (len < 8) {
        LOG(WARN) << "KNX telegram is not long enough";
        return;
    }
    std::string prefix = "/devices/knx/controls/data";
    std::stringstream ss;
    unsigned acpi = (buf[6] & 0x3) << 2 | (buf[7] >> 6);
    int dataLen = ((buf[5] & 0xf));
    uint16_t srcAddr = (buf[1] << 8 | buf[2]);
    uint16_t dstAddr = (buf[3] << 8 | buf[4]);
    // Source address is always individual
    ss << "i:" << (srcAddr >> 12) << "/" << ((srcAddr >> 8) & 0xf);
    ss << "/" << (srcAddr & 0xff) << " ";
    if (buf[5] >> 7) {
        // group address
        ss << "g:" << ((dstAddr >> 11) & 0xf) << "/" << ((dstAddr >> 8) & 0x7);
        ss << "/" << (dstAddr & 0xff);
    } else {
        // individual address
        ss << "i:" << (dstAddr >> 12) << "." << ((dstAddr >> 8) & 0xf) << "." << (dstAddr & 0xff);
    }
    ss << std::hex << std::setfill('0');
    ss << " " << names[acpi] << " ";
    if (dataLen == 2) {
        ss << "0x" << std::setw(2) << (buf[7] & 0x3f);
    } else {
        if (len != dataLen + 8) {
            LOG(WARN) << "KNX telegram has inconsistent length";
            return;
        }
        ss << "0x" << std::setw(2) << (buf[7] & 0x3f) << " ";
        for (int i = 8; i < dataLen + 7; i++) {
            ss << "0x" << std::setw(2) << (unsigned)buf[i] << " ";
        }
    }
    MqttClient->Publish(NULL, prefix, ss.str());
}
