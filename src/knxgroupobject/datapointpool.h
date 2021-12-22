#pragma once

#include "idpt.h"

namespace knx
{
    namespace object
    {
        namespace DataPointPool
        {
            /// \brief Create DataPoint by name.
            /// If the name is not found, a default datapoint is created.
            /// \param name DataPoint name
            /// \return  Pointer to a DataPoint object
            PDpt MakeDataPointByName(const std::string& name);

            /// \brief Finding the system name of a datapoint by number in the registry of datapoints X.XXX
            /// \param general Main group before point
            /// \param sub Subgroup after point
            /// \return DataPoint name
            std::string GetDataPointNameById(uint32_t general, uint32_t sub);
        }
    }
}
