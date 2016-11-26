#include "knx-client.h"
#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include "wbmqtt/utils.h"
#include <cstring>
#include "observer.h"

extern bool debug;

TKNXClient::TKNXClient() {
    int res;
    src_addr = ((5) << 11) + (3<< 8) + (1); //read this from config
    out = EIBSocketLocal("/tmp/eib");
    res = EIBOpenT_TPDU(out, src_addr); // we may want to open another type of connection some day
    if(res) ; //throw exception here
    in = EIBSocketRemote("localhost", 6720);
    res = EIBOpenVBusmonitor(in);
    if(res) ; //throw something
}

void TKNXClient::Observe(PMQTTKnxObserver observer) {
    Observer = observer;
}

TKNXClient::~TKNXClient() {
    if(out) EIBClose(out);
    if(in) EIBClose(in);
}
int TKNXClient::PreparePayload(uint8_t *buf, std::string payload) {
    uint8_t ACPI = 0x0b; // User Message
    buf[0] = 0x0; // UDP (Unnumbered Data Packet (0b00) and 0b0000 as unused number)
    buf[0] |= ACPI >> 2;
    buf[1] = 0x0;
    buf[1] |= ((ACPI & 0x3) << 6);
    std::memcpy(buf+2, payload.c_str(), payload.size());
    return payload.size()+2;
}

void TKNXClient::Send(std::string topic, std::string payload) {
    std::vector<std::string> tokens = StringSplit(topic, "/");

    int res;

    if(tokens[1] != "devices" || tokens[2] != "knx") return; // or error, handle it later

    uint8_t buf[MAX_PAYLOAD_SIZE];
    int len = PreparePayload(buf, payload);

    if(tokens[3] == "group"){
        eibaddr_t addr = (std::stoi(tokens[4]) << 10) + (std::stoi(tokens[5])<< 8) + (std::stoi(tokens[6]));

        res = EIBSendGroup(out, addr, len, buf);
        if(res == -1) ; // throw exception here

    }
    else{
        eibaddr_t addr = (std::stoi(tokens[3]) << 11) + (std::stoi(tokens[4])<< 8) + (std::stoi(tokens[5]));

        res = EIBSendTPDU(out, addr, len, buf);
        if(res == -1) ; // throw exception here
    }
}

void TKNXClient::Loop() {

    while (true) {
        int fd = EIB_Poll_FD(in);
        if(fd==-1) ; //throw error here

        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);

        int ret = select(fd+1, &set, NULL, NULL, NULL);
        if(ret != 1) ; // throw exception here

        uint8_t packet[2048];
        ret = EIBGetBusmonitorPacket(in, 2048, packet);

        if(ret==-1); //handle error
        HandlePacket(packet, ret);
    }
}

void TKNXClient::HandlePacket(uint8_t *buf, int len){
    for(int i = 0; i < len; i++){
        std::cout << (int)buf[i]<< " ";
    }
    std::cout << "\n";
    Observer->OnPackage(buf, len);
}
