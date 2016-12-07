#include "knx-client.h"
#include "observer.h"
#include "wbmqtt/utils.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>

#include "logging.h"

TKnxClient::TKnxClient(std::string url)
{
    Out = EIBSocketURL(url.c_str());
    In = EIBSocketURL(url.c_str());
    if (!Out || !In) throw TKnxException("failed to open url: " + url);
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

eibaddr_t TKnxClient::ParseKnxAddress(const std::string& addr, bool isGroup)
{
    try {
        std::vector<std::string> tokens = StringSplit(addr, "/");
        if (isGroup) {
            if (tokens.size() == 2) {
                uint16_t main = std::stoi(tokens[0]);
                uint16_t sub = std::stoi(tokens[1]);

                return ((main & 0xf) << 11) | (sub & 0x7ff);
            } else if (tokens.size() == 3) {
                uint16_t main = std::stoi(tokens[0]);
                uint16_t middle = std::stoi(tokens[1]);
                uint16_t sub = std::stoi(tokens[2]);

                return ((main & 0xf) << 11) | ((middle & 0x7) << 8) | (sub & 0xff);
            }
        } else {
            if (tokens.size() == 3) {
                uint16_t area = std::stoi(tokens[0]);
                uint16_t line = std::stoi(tokens[1]);
                uint16_t device = std::stoi(tokens[2]);

                return ((area & 0xf) << 12) | ((line & 0xf) << 8) | (device & 0xff);
            }
        }
    } catch (std::exception& e) {
    }

    throw TKnxException("invalid address: " + addr);
}

void TKnxClient::SendTelegram(std::string payload)
{
    // payload should be in form of DestAddr ACPI data
    std::stringstream ss(payload);
    std::string addrStr;
    std::string data;
    uint8_t acpi;
    eibaddr_t srcAddr = 0;
    eibaddr_t destAddr = 0;
    ss >> addrStr >> acpi;
    acpi &= 0x7;

    ss.ignore(1); // Ignore one space that delimits ACPI and data
    std::getline(ss, data);

    bool isGroup = (addrStr[0] == 'g');
    if (isGroup) {
        destAddr = ParseKnxAddress(addrStr.substr(2), isGroup);
    } else {
        std::string::size_type pos = addrStr.find(':', 2);
        srcAddr = ParseKnxAddress(addrStr.substr(2, pos - 2), isGroup);
        destAddr = ParseKnxAddress(addrStr.substr(pos + 1), isGroup);
    }

    LOG(INFO) << "KNX Client send telegram to: " << std::hex << std::showbase << destAddr
              << " with ACPI: " << (unsigned)acpi << " with data: " << data;

    uint8_t telegram[MAX_TELEGRAM_LENGTH] = {0};
    telegram[0] = 0x0; // UDP (Unnumbered Data Packet (0b00) and 0b0000 as unused number)
    telegram[0] |= acpi >> 2;
    telegram[1] = 0x0;
    telegram[1] |= ((acpi & 0x3) << 6);
    std::memcpy(telegram + 2, data.c_str(), data.size());

    int res;
    res = EIBReset(Out);
    if (res == -1) throw TKnxException("failed to reset connection");

    if (isGroup) {
        res = EIBOpen_GroupSocket(Out, 0);
        if (res == -1) throw TKnxException("failed to open GroupSocket");
        res = EIBSendGroup(Out, destAddr, data.size() + 2, telegram);
        if (res == -1) throw TKnxException("failed to send group telegram");
    } else {
        res = EIBOpenT_TPDU(Out, srcAddr);
        if (res == -1) throw TKnxException("failed to open TPDU connection");
        res = EIBSendTPDU(Out, destAddr, data.size() + 2, telegram);
        if (res == -1) throw TKnxException("failed to send group telegram");
    }
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

        int len = EIBGetBusmonitorPacket(In, MAX_TELEGRAM_LENGTH, telegram);
        if (len == -1) throw TKnxException("failed to read Busmonitor packet");
        std::stringstream ss;
        ss << "KNX Client received a telegram: ";
        ss << std::hex << std::noshowbase;
        for (int i = 0; i < len; i++) {
            ss << "0x" << std::setw(2) << std::setfill('0');
            ss << (unsigned)telegram[i] << " ";
        }
        LOG(INFO) << ss.str();

        Observer->OnTelegram(telegram, len);
    }
}
