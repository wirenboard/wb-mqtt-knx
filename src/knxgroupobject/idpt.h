#pragma once

#include "wblib/any.h"

namespace knx
{
    namespace object
    {
        /// Description of the MQTT Control associated with KNX field
        struct DptDescriptorField
        {
            std::string Id;   ///< Postfix ID for a MQTT control. Example: ControlId_DescriptorId
            std::string Type; ///< Control value type
            double Min;       ///< Minimum value for a control
            double Max;       ///< Maximum value for a control
        };

        /// \brief A set of rules for converting KNX <-> MQTT messages
        ///
        /// A set of rules for converting KNX <-> MQTT messages and caching the value of the group object.
        /// Contains a description of mqtt controls for the high-level user interface.
        /// A group object in mqtt is represented by a set of controls
        /// according to the number and type of fields in the data point.
        class IDpt
        {
        public:
            /// Get a set of field descriptors for creating mqtt controls
            /// \return set of field descriptors
            virtual std::vector<DptDescriptorField> getDescriptor() const = 0;

            /// Receiving and cache a value from the mqtt control associated with the datapoint field
            /// \param controlIndex sequence number of the MQTT control from the descriptor
            /// \param value MQTT control value
            virtual void FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value) = 0;

            /// Receiving and cache a message from a KNX network
            /// \param payload Content of APDU from KNX telegram
            virtual void FromKnx(const std::vector<uint8_t>& payload) = 0;

            /// Converting data for sending to KNX
            /// \return APDU payload
            virtual std::vector<uint8_t> ToKnx() = 0;

            /// Converting data for sending to MQTT
            /// \return List of control values. The position in the list corresponds to the position in the list of
            ///         field descriptors
            virtual std::vector<WBMQTT::TAny> ToMqtt() = 0;

            /// Virtual destructor
            virtual ~IDpt() = default;
        };
    }
}