#ifndef WB_MQTT_KNX_KNXCONVERTER_H
#define WB_MQTT_KNX_KNXCONVERTER_H

#include "knxtelegram.h"
#include <memory>

namespace knx
{
    class TKnxConverter
    {
    public:
        static std::string KnxTelegramToMqtt(const TTelegram& telegram);
        static std::shared_ptr<TTelegram> MqttToKnxTelegram(const std::string& payload);

    private:
        static std::string KnxSourceAddressToString(const TTelegram& telegram);
        static std::string KnxReceiverAddressToString(const TTelegram& telegram);
        static std::string KnxApciToString(const TTelegram& telegram);
        static std::string KnxPayloadToString(const TTelegram& telegram);

        static eibaddr_t StringToKnxAddress(const std::string& addr, bool groupBit);

        static std::tuple<bool, knx::telegram::TApci> StringToKnxApci(const std::string& apciString);

        static uint8_t StringToByte(const std::string& byte);
    };
} // namespace knx

#endif // WB_MQTT_KNX_KNXCONVERTER_H
