#pragma once

#include "knxtelegram.h"
#include <memory>

namespace knx
{
    namespace converter
    {
        std::string KnxTelegramToMqtt(const TTelegram& telegram);
        PTelegram MqttToKnxTelegram(const std::string& payload);
    }
}
