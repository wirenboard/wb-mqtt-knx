#pragma once

#include "igroupobject.h"

namespace knx
{
    namespace object
    {
        /// The MQTT control parameters for creating group objects
        struct TGroupObjectMqttParameter
        {
            std::string Type;         ///< Type of MQTT control
            std::string ControlId;    ///< Control ID prefix
            std::string ControlTitle; ///< Control title
            bool isReadOnly = false;  ///< MQTT Control read only flag
        };

        /// The MQTT-related group object builder
        class IGroupObjectMqttBuilder
        {
        public:
            /// Create and link MQTT Logical Device with the generated group objects
            /// \param id MQTT Device ID
            /// \param title MQTT Device Title
            virtual void LinkDevice(const std::string& id, const std::string& title) = 0;

            /// Create Group Object with linked MQTT Device
            /// \param parameter MQTT controls parameters
            /// \return the Group Object smart pointer
            virtual PGroupObject Create(const TGroupObjectMqttParameter& parameter) = 0;

            /// Remove unused MQTT control. It is used to clean up unused controls that may remain after previous use.
            virtual void RemoveUnusedControls() = 0;

            /// Remove all builder-created MQTT devices
            virtual void Clear() = 0;

            /// Virtual destructor
            virtual ~IGroupObjectMqttBuilder() = default;
        };
    }
}
