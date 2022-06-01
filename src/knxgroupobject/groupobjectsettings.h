#pragma once

#include "../knxgroupaddress.h"
#include <experimental/optional>

namespace knx
{
    /// Group object settings
    struct TGroupObjectSettings
    {
        std::string DatapointType; ///< Datapoint type name
        std::string ControlId;     ///< Control ID prefix
        std::string ControlTitle;  ///< Control title
        bool IsReadOnly{false};    ///< MQTT Control read only flag

        TKnxGroupAddress GroupAddress;                                      // Group address
        std::experimental::optional<TKnxGroupAddress> FeedbackGroupAddress; // Feedback Group Address if exist

        bool ReadRequestAfterStart{false}; ///< Send a read request after the start of the knxd client
        std::chrono::milliseconds ReadRequestPollInterval{0}; ///< Period of sending a read request
        std::chrono::milliseconds ReadResponseTimeout{0};     ///< Time to wait for a response to a read request
                                                              ///< Must be less than the polling period.
    };
}