#include "Telegram.h"
#include "logging.h"
#include <iomanip>
#include <sstream>
#include <wbmqtt/utils.h>

std::vector<std::string> TKnxTelegram::ApciNames = {"GroupValueRead",
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

eibaddr_t TKnxTelegram::ParseKnxAddress(const std::string& addr)
{
    std::vector<std::string> tokens;

    try {
        tokens = StringSplit(addr, "/");
        if (GroupBit) {
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

unsigned TKnxTelegram::ParseByte(std::string byte)
{
    if (byte.empty()) throw TKnxException("Trying to read a byte from empty string");

    try {
        if (byte.substr(0, 2) == "0b" || byte.substr(0, 2) == "0B") {
            if (byte.length() == 2) throw TKnxException("invalid byte: " + byte);
            char* strEnd;
            unsigned res = std::strtoul(byte.c_str() + 2, &strEnd, 2);
            if (*strEnd != 0) throw TKnxException("invalid byte: " + byte);
            return res;
        }

        std::size_t num;
        int ret = std::stoi(byte, &num, 0);
        if (num != byte.length()) throw TKnxException("invalid byte: " + byte);
        return ret;

    } catch (TKnxException& e) {
        throw;
    } catch (std::exception& e) {
        throw TKnxException("invalid byte: " + byte);
    }
}

TKnxTelegram::TKnxTelegram(std::string mqttPayload)
{
    // payload should be in form of DestAddr APCI data
    std::stringstream ss(mqttPayload);
    std::string addrStr;
    std::string apciStr;
    ss >> addrStr >> apciStr;
    unsigned apci = 0;
    for (; apci < ApciNames.size(); apci++) {
        if (ApciNames[apci] == apciStr) break;
    }
    if (apci == ApciNames.size()) {
        // try to read APCI as number
        try {
            apci = (ParseByte(apciStr)) & 0xf;
        } catch (TKnxException& e) {
            throw TKnxException("Unknown telegram type: " + apciStr);
        }
    }
    GroupBit = (addrStr[0] == 'g');
    if (GroupBit) {
        SrcAddr = 0;
        DstAddr = ParseKnxAddress(addrStr.substr(2));
    } else {
        std::string::size_type pos = addrStr.find(':', 2);
        SrcAddr = ParseKnxAddress(addrStr.substr(2, pos - 2));
        DstAddr = ParseKnxAddress(addrStr.substr(pos + 1));
    }

    PayloadData[0] = 0x0; // UDP (Unnumbered Data Packet (0b00) and 0b0000 as unused number)
    PayloadData[0] |= apci >> 2;
    PayloadData[1] = 0x0;
    PayloadData[1] |= ((apci & 0x3) << 6);
    unsigned shortdata = 0;
    std::string shortDataStr;
    if (ss >> shortDataStr) {
        shortdata = ParseByte(shortDataStr);
    }

    PayloadData[1] |= (shortdata & 0x3f);
    Size = 2;
    unsigned data;
    std::string dataStr;
    while (ss >> std::hex >> dataStr) {
        data = ParseByte(dataStr);
        PayloadData[Size++] = (data & 0xff);
        if (Size == MAX_PAYLOAD_SIZE) throw TKnxException("Telegram is too long.");
    }
}

TKnxTelegram::TKnxTelegram(uint8_t* knxBuffer, int len)
{
    if (len < 8) {
        throw TKnxException("KNX telegram is not long enough");
    }
    std::stringstream ss;
    unsigned apci = (knxBuffer[6] & 0x3) << 2 | (knxBuffer[7] >> 6);
    Size = ((knxBuffer[5] & 0xf));
    SrcAddr = (knxBuffer[1] << 8 | knxBuffer[2]);
    DstAddr = (knxBuffer[3] << 8 | knxBuffer[4]);

    // Source address is always individual
    ss << "i:" << (SrcAddr >> 12) << "/" << ((SrcAddr >> 8) & 0xf);
    ss << "/" << (SrcAddr & 0xff) << " ";

    GroupBit = knxBuffer[5] >> 7;

    if (GroupBit) {
        // group address
        ss << "g:" << ((DstAddr >> 11) & 0xf) << "/" << ((DstAddr >> 8) & 0x7);
        ss << "/" << (DstAddr & 0xff);
    } else {
        // individual address
        ss << "i:" << (DstAddr >> 12) << "/" << ((DstAddr >> 8) & 0xf);
        ss << "/" << (DstAddr & 0xff);
    }

    ss << " " << ApciNames[apci] << " ";

    ss << std::hex << std::setfill('0');
    if (Size == 1) {
        // Short data, return last 6 bit of the last octet
        ss << "0x" << std::setw(2) << (knxBuffer[7] & 0x3f);
    } else {
        if (len != Size + 8) {
            throw TKnxException("KNX telegram has inconsistent length");
        }

        // "Long" format skip that 6 bits and store data starting from the next octet
        for (int i = 8; i < len - 1; i++) {
            ss << "0x" << std::setw(2) << (unsigned)knxBuffer[i] << " ";
        }
    }
    PayloadString = ss.str();
    Size++;
}

bool TKnxTelegram::IsGroup()
{
    return GroupBit;
}

eibaddr_t TKnxTelegram::GetDstAddr()
{
    return DstAddr;
}

eibaddr_t TKnxTelegram::GetSrcAddr()
{
    return SrcAddr;
}

int TKnxTelegram::GetSize()
{
    return Size;
}

uint8_t* TKnxTelegram::GetKnxPayload()
{
    return PayloadData;
}

std::string TKnxTelegram::GetMqttPayload()
{
    return PayloadString;
}
