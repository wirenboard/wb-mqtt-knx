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

            /// Is there a datapoint with this id
            /// \param datapointId datapoint id
            /// \retval true if true if contained in builder
            virtual bool HasDpt(const TDatapointId& datapointId) = 0;
        };
    }
}