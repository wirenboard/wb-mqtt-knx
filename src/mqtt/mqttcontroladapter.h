#pragma once
#include "imqttcontroladapter.h"
#include <wblib/wbmqtt.h>

#include <utility>

namespace mqtt
{
    class MqttControlAdapter: public IMqttControlAdapter
    {
    public:
        explicit MqttControlAdapter(std::shared_ptr<WBMQTT::TControl> control);

        explicit MqttControlAdapter() = delete;
        virtual ~MqttControlAdapter() = default;

        void Send(WBMQTT::TAny&& value) override;
        void SetEventHandler(const std::function<void(const WBMQTT::TAny&)>& f) override;

    private:
        std::shared_ptr<WBMQTT::TControl> Control;
    };
}
