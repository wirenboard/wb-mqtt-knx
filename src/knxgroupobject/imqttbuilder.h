#pragma once

#include "igroupobject.h"

namespace knx
{
    namespace object
    {
        struct TGroupObjectMqttParameter
        {
            std::string Type;
            std::string ControlId;
            std::string ControlTitle;
            bool isReadOnly = false;
        };

        class IGroupObjectMqttBuilder
        {
        public:
            virtual void LinkDevice(const std::string& id, const std::string& name) = 0;
            virtual PGroupObject Create(const TGroupObjectMqttParameter& parameter) = 0;
            virtual void RemoveUnusedControls() = 0;
            virtual void Clear() = 0;

            virtual ~IGroupObjectMqttBuilder() = default;
        };
    }
}
