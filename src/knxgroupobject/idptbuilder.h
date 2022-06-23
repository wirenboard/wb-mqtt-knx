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

            virtual ~IDptBuilder() = default;
        };
    }
}