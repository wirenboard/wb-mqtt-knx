#pragma once

#include "knxtelegram.h"
#include <memory>

namespace knx
{
    namespace converter
    {
        std::string KnxTelegramToMqtt(const TTelegram& telegram);
        std::shared_ptr<TTelegram> MqttToKnxTelegram(const std::string& payload);
    }
}
