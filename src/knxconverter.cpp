#include "knxconverter.h"
#include "knxexception.h"
#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>
#include <wblib/utils.h>

using namespace knx;

namespace
{
    const std::array<const char*, 16> ApciString = {"GroupValueRead",
                                                    "GroupValueResponse",
                                                    "GroupValueWrite",
                                                    "IndividualAddrWrite",
                                                    "IndividualAddrRequest",
                                                    "IndividualAddrResponse",
                                                    "AdcRead",
                                                    "AdcResponse",
                                                    "MemoryRead",
                                                    "MemoryResponse",
                                                    "MemoryWrite",
                                                    "UserMessage",
                                                    "MaskVersionRead",
                                                    "MaskVersionResponse",
                                                    "Restart",
                                                    "Escape"};

    const std::array<const char*, 4> CommunicationTypeString = {"Connect", "Disconnect", "Ack", "Nack"};

    // KnxTelegram Convertors

    std::string KnxApciToString(const TTelegram& telegram)
    {
        return ApciString.at(static_cast<uint32_t>(telegram.Tpdu().GetAPCI()));
    }

    std::string KnxCommunicationTypeToString(const TTelegram& telegram)
    {
        return CommunicationTypeString.at(static_cast<uint32_t>(telegram.Tpdu().GetControlDataType()));
    }

    std::string KnxSourceAddressToString(const TTelegram& telegram)
    {
        std::stringstream ss;
        auto address = telegram.GetSourceAddress();

        // Source address is always individual
        ss << "i:" << (address >> 12) << "/" << ((address >> 8) & 0xf);
        ss << "/" << (address & 0xff);
        return ss.str();
    }

    std::string KnxReceiverAddressToString(const TTelegram& telegram)
    {
        std::stringstream ss;

        auto receiverAddress = telegram.GetReceiverAddress();

        if (telegram.IsGroupAddressed()) {
            // group address
            ss << "g:" << ((receiverAddress >> 11) & 0xf) << "/" << ((receiverAddress >> 8) & 0x7);
            ss << "/" << (receiverAddress & 0xff);
        } else {
            // individual address
            ss << "i:" << (receiverAddress >> 12) << "/" << ((receiverAddress >> 8) & 0xf);
            ss << "/" << (receiverAddress & 0xff);
        }

        return ss.str();
    }

    std::string KnxPayloadToString(const TTelegram& telegram)
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        auto payload = telegram.Tpdu().GetPayload();
        auto payloadSize = payload.size();

        if (payloadSize == 1) {
            // Short data, return last 6 bit of the last octet
            ss << "0x" << std::setw(2) << static_cast<uint32_t>(payload[0]);
        } else {
            // "Long" format skip that 6 bits and store data starting from the next octet
            for (uint32_t i = 1; i < payloadSize; ++i) {
                ss << "0x" << std::setw(2) << static_cast<uint32_t>(payload[i]) << " ";
            }
        }
        return ss.str();
    }

    // String convertors

    eibaddr_t StringToKnxAddress(const std::string& addr, bool groupBit)
    {
        std::vector<std::string> tokens;

        try {
            tokens = WBMQTT::StringSplit(addr, "/");
            if (groupBit) {
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

        wb_throw(TKnxException, "invalid address: " + addr);
    }

    uint8_t StringToByte(const std::string& byte)
    {
        if (byte.empty())
            wb_throw(TKnxException, "Trying to read a byte from empty string");

        try {
            if (byte.substr(0, 2) == "0b" || byte.substr(0, 2) == "0B") {
                if (byte.length() == 2)
                    wb_throw(TKnxException, "invalid byte: " + byte);
                char* strEnd;
                unsigned res = std::strtoul(byte.c_str() + 2, &strEnd, 2);
                if (*strEnd != 0)
                    wb_throw(TKnxException, "invalid byte: " + byte);
                return res;
            } else {
                std::size_t num;
                int ret = std::stoi(byte, &num, 0);
                if (num != byte.length())
                    wb_throw(TKnxException, "invalid byte: " + byte);
                return ret;
            }
        } catch (TKnxException& e) {
            throw;
        } catch (std::exception& e) {
            wb_throw(TKnxException, "invalid byte: " + byte);
        }
    }

    std::tuple<bool, knx::telegram::TApci> StringToKnxApci(const std::string& apciString)
    {
        auto it = std::find(ApciString.begin(), ApciString.end(), apciString);

        if (it != ApciString.end()) {
            return {true, static_cast<telegram::TApci>(std::distance(ApciString.begin(), it))};
        } else {
            return {false, {}};
        }
    }
}

std::string converter::KnxTelegramToMqtt(const TTelegram& telegram)
{
    std::stringstream ss;

    ss << KnxSourceAddressToString(telegram) << " " << KnxReceiverAddressToString(telegram) << " ";

    auto transportLayerType = telegram.Tpdu().GetCommunicationType();
    if ((transportLayerType == telegram::TCommunicationType::UDP) ||
        (transportLayerType == telegram::TCommunicationType::NDP))
    {
        ss << KnxApciToString(telegram) << " " << KnxPayloadToString(telegram);
    } else {
        ss << KnxCommunicationTypeToString(telegram);
    }

    return ss.str();
}

std::shared_ptr<TTelegram> converter::MqttToKnxTelegram(const std::string& payload)
{
    auto telegram = std::make_shared<knx::TTelegram>();

    // payload should be in form of DestAddr APCI data
    std::stringstream ss(payload);
    std::string addrStr;
    std::string apciStr;
    ss >> addrStr >> apciStr;

    bool isFoundAcpi;
    knx::telegram::TApci acpiCode;

    std::tie(isFoundAcpi, acpiCode) = StringToKnxApci(apciStr);

    if (isFoundAcpi) {
        telegram->Tpdu().SetAPCI(acpiCode);
    } else {
        // try to read APCI as number
        try {
            const auto apci = static_cast<telegram::TApci>((StringToByte(apciStr)) & 0xf);
            telegram->Tpdu().SetAPCI(apci);
        } catch (TKnxException& e) {
            wb_throw(TKnxException, "Unknown telegram type: " + apciStr);
        }
    }

    const auto groupBit = (addrStr[0] == 'g');
    telegram->SetGroupAddressedFlag(groupBit);

    if (groupBit) {
        telegram->SetSourceAddress(0);
        telegram->SetReceiverAddress(StringToKnxAddress(addrStr.substr(2), true));
    } else {
        std::string::size_type pos = addrStr.find(':', 2);

        telegram->SetSourceAddress(StringToKnxAddress(addrStr.substr(2, pos - 2), false));
        telegram->SetReceiverAddress(StringToKnxAddress(addrStr.substr(pos + 1), false));
    }

    std::vector<uint8_t> knxPayload = {0};
    std::string dataStr;

    if (ss >> dataStr) {
        knxPayload[0] = StringToByte(dataStr) & 0x3F;
    }

    uint32_t size = 1;
    while (ss >> std::hex >> dataStr) {
        knxPayload.push_back(StringToByte(dataStr));
        ++size;
        if (size > knx::TTpdu::MaxPayloadSize)
            wb_throw(TKnxException, "Telegram payload is too long.");
    }
    telegram->Tpdu().SetPayload(knxPayload);

    return telegram;
}