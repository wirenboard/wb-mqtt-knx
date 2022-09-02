#include "knxconverter.h"
#include "knxexception.h"
#include "knxgroupaddress.h"
#include "knxindividualaddress.h"
#include "knxutils.h"
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
        TKnxIndividualAddress sourceAddress{telegram.GetSourceAddress()};

        // Source address is always individual
        ss << "i:" << sourceAddress.ToString('/');
        return ss.str();
    }

    std::string KnxReceiverAddressToString(const TTelegram& telegram)
    {
        std::stringstream ss;

        auto receiverAddress = telegram.GetReceiverAddress();

        if (telegram.IsGroupAddressed()) {
            // group address
            TKnxGroupAddress groupAddress{receiverAddress};
            ss << "g:" << groupAddress.ToString();
        } else {
            // individual address
            TKnxIndividualAddress individualAddress{receiverAddress};
            ss << "i:" << individualAddress.ToString('/');
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
        eibaddr_t address = 0;
        std::string errorMessage = "Invalid address: " + addr;

        try {
            if (groupBit) {
                TKnxGroupAddress groupAddress(addr);
                address = groupAddress.GetEibAddress();
            } else {
                auto tokens = WBMQTT::StringSplit(addr, "/");
                if (tokens.size() == 3) {
                    uint16_t area = std::stoi(tokens[0]);
                    uint16_t line = std::stoi(tokens[1]);
                    uint16_t device = std::stoi(tokens[2]);

                    address = ((area & 0xf) << 12) | ((line & 0xf) << 8) | (device & 0xff);
                } else {
                    wb_throw(TKnxException, errorMessage);
                }
            }
        } catch (std::exception& e) {
            wb_throw(TKnxException, errorMessage);
        }

        return address;
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

PTelegram converter::MqttToKnxTelegram(const std::string& payload)
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
            const auto apci = static_cast<telegram::TApci>((utils::StringValueToByte(apciStr)) & 0xf);
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
        knxPayload[0] = utils::StringValueToByte(dataStr) & 0x3F;
    }

    uint32_t size = 1;
    while (ss >> std::hex >> dataStr) {
        knxPayload.push_back(utils::StringValueToByte(dataStr));
        ++size;
        if (size > knx::TTpdu::MaxPayloadSize)
            wb_throw(TKnxException, "Telegram payload is too long.");
    }
    telegram->Tpdu().SetPayload(knxPayload);

    return telegram;
}