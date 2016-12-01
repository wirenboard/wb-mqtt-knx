#include "knx-client.h"
#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include "wbmqtt/utils.h"
#include <cstring>
#include "observer.h"
#include <sstream>

extern bool debug;

TKNXClient::TKNXClient() {
    int res;
    src_addr = ((5) << 11) + (3<< 8) + (1); //read this from config
    out = EIBSocketLocal("/tmp/eib");
    //res = EIBOpenT_TPDU(out, src_addr); // we may want to open another type of connection some day
    if(res) ; //throw exception here
    in = EIBSocketRemote("localhost", 6720);
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

void TKNXClient::Send(std::string payload) {
    // payload will be in form of 1/2/3 data
    // data should contain correct ACPI
    //std::cout << payload << std::endl;
    std::stringstream ss(payload);
    std::string address;
    std::string data;
    uint8_t ACPI;
    ss >> address >> ACPI >> data;
    //std::cout << address << data << std::endl;
    std::vector<std::string> tokens = StringSplit(address, "/");
    eibaddr_t addr = ((std::stoi(tokens[0]) & 0x7) << 11) + ((std::stoi(tokens[1])&0x3)<< 8) + (std::stoi(tokens[2])&0xff);
    //std::cout << addr << std::endl;
    uint8_t buf[32];
    buf[0] = 0x0; // UDP (Unnumbered Data Packet (0b00) and 0b0000 as unused number)
    buf[0] |= ACPI >> 2;
    buf[1] = 0x0;
    buf[1] |= ((ACPI & 0x3) << 6);
    std::memcpy(buf+2, data.c_str(), data.size());

    int res;
    res = EIBOpen_GroupSocket(out, 0);
    res = EIBSendGroup(out, addr, data.size()+2, buf);
    res = EIBReset(out);
}

void TKNXClient::Loop() {
    int res = EIBOpenVBusmonitor(in);

    while (true) {
        int fd = EIB_Poll_FD(in);
        if(fd==-1) ; //throw error here

        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);

        int ret = select(fd+1, &set, NULL, NULL, NULL);
        if(ret != 1) ; // throw exception here

        uint8_t packet[2048] = {0};
        ret = EIBGetBusmonitorPacket(in, 2048, packet);

        if(ret==-1); //handle error
        Observer->OnPackage(packet, ret);
    }
}

void TKNXClient::HandlePacket(uint8_t *buf, int len){
    for(int i = 0; i < len; i++){
        std::cout << (int)buf[i]<< " ";
    }
    std::cout << "\n";
    Observer->OnPackage(buf, len);
}
