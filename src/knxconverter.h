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
    };
} // namespace knx

#endif // WB_MQTT_KNX_KNXCONVERTER_H
