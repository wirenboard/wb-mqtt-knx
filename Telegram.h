#include "Exception.h"
#include <eibclient.h>
#include <string>
#include <vector>

#ifndef TELEGRAM_H
#define TELEGRAM_H
#define MAX_PAYLOAD_SIZE 254
class TKnxTelegram {
public:
    TKnxTelegram(std::string mqttPayload);
    TKnxTelegram(uint8_t* knxBuffer, int len);

    // MQTT->KNX accessors
    bool IsGroup();
    eibaddr_t GetSrcAddr();
    eibaddr_t GetDstAddr();
    int GetSize();
    uint8_t* GetKnxPayload();

    // KNX->MQTT accessors
    std::string GetMqttPayload();

private:
    eibaddr_t ParseKnxAddress(const std::string& addr);
    static unsigned ParseByte(std::string byte);

    bool GroupBit;
    eibaddr_t SrcAddr;
    eibaddr_t DstAddr;
    int Size;
    std::string PayloadString;

    uint8_t PayloadData[MAX_PAYLOAD_SIZE];

    static std::vector<std::string> ApciNames;
};

#endif // TELEGRAM_H
