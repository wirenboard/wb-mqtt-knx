#pragma once
#include "datapointid.h"
#include "idpt.h"

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
            /// \return Pointer to the created datapoint
            virtual PDpt Create(const TDatapointId& datapointId) = 0;
        };
    }
}