#pragma once

#include "base.h"

namespace knx
{
    namespace object
    {
        struct TGroupObjectMqttParameter
        {
            std::string Type;
            std::string ControlId;
            std::string ControlTitle;
        };

        class IGroupObjectMqttBuilder
        {
        public:
            virtual void LinkDevice(const std::string& id, const std::string& name) = 0;
            virtual std::shared_ptr<TGroupObjectBase> Create(const TGroupObjectMqttParameter& parameter) = 0;
        };
    }
}
