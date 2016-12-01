#include "knx-client.h"
#include "observer.h"
#include "wbmqtt/utils.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>

TKnxClient::TKnxClient(std::string url) : Debug(false)
{
    Out = EIBSocketURL(url.c_str());
    In = EIBSocketURL(url.c_str());
    if (!Out || !In) throw TKnxException("failed to open url: " + url);
}

bool TKnxClient::SetDebug(bool debug)
{
    bool ret = Debug;
    Debug = debug;
    return ret;
}

void TKnxClient::Observe(PMqttKnxObserver observer)
{
    Observer = observer;
}

TKnxClient::~TKnxClient()
{
    if (Out) EIBClose(Out);
    if (In) EIBClose(In);
}

void TKnxClient::SendTelegram(std::string payload)
{
    // payload should be in form of DestAddr ACPI data
    std::stringstream ss(payload);
    eibaddr_t destAddr;
    std::string data;
    uint8_t acpi;
    ss >> destAddr >> acpi >> data;
    acpi &= 0x7;

    if (Debug) {
        std::cout << "KNX Client send telegram to: " << std::hex << std::showbase << destAddr;
        std::cout << " with ACPI: " << acpi << " with data: " << data << std::endl;
    }

    uint8_t telegram[MAX_TELEGRAM_LENGTH] = {0};
    telegram[0] = 0x0; // UDP (Unnumbered Data Packet (0b00) and 0b0000 as unused number)
    telegram[0] |= acpi >> 2;
    telegram[1] = 0x0;
    telegram[1] |= ((acpi & 0x3) << 6);
    std::memcpy(telegram + 2, data.c_str(), data.size());

    int res;
    res = EIBOpen_GroupSocket(Out, 0);
    if (res == -1) throw TKnxException("failed to open GroupSocket");
    res = EIBSendGroup(Out, destAddr, data.size() + 2, telegram);
    if (res == -1) throw TKnxException("failed to send group telegram");
    res = EIBReset(Out);
    if (res == -1) throw TKnxException("failed to reset connection");
}

void TKnxClient::Loop()
{

    int res = EIBOpenVBusmonitor(In);
    if (res == -1) throw TKnxException("failed to open Busmonitor connection");
    while (true) {
        int fd = EIB_Poll_FD(In);
        if (fd == -1) throw TKnxException("failed to get Poll fd");

        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);

        res = select(fd + 1, &set, NULL, NULL, NULL);
        if (res == -1) throw TKnxException(std::string("select failed: ") + std::strerror(errno));

        uint8_t telegram[MAX_TELEGRAM_LENGTH] = {0};

        int len = EIBGetBusmonitorPacket(In, 255, telegram);
        if (len == -1) throw TKnxException("failed to read Busmonitor packet");
        if (Debug) {
            std::cout << "KNX Client received telegram: ";
            std::cout << std::hex << std::noshowbase;
            for (int i = 0; i < len; i++) {
                std::cout << "0x" << std::setw(2) << std::setfill('0');
                std::cout << (unsigned)telegram[i] << " ";
            }
            std::cout << std::endl;
        }

        Observer->OnTelegram(telegram, len);
    }
}
