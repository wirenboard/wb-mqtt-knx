#pragma once
#include "datapointid.h"
#include "idpt.h"
#include <experimental/optional>

namespace knx
{
    namespace object
    {
        /// \brief Interface for creating a data point from ID
        class IDptBuilder
        {
        public:
            /// Create a data point from ID
            /// \param datapointId datapoint Id for creating
            /// \return optional pointer to the created datapoint. Or std::nullopt if datapoint not found
            virtual std::experimental::optional<PDpt> Create(const TDatapointId& datapointId) = 0;

            /// Is there a datapoint with this id
            /// \param datapointId datapoint id
            /// \retval true if contained in builder
            virtual bool HasDpt(const TDatapointId& datapointId) const = 0;

            /// Get a datapoint config name from Id
            /// \param datapointId datapoint Id
            /// \return optional datapoint name. Or std::nullopt if datapoint not found
            virtual std::experimental::optional<std::string> GetDptConfigName(
                const TDatapointId& datapointId) const = 0;
        };
    }
}