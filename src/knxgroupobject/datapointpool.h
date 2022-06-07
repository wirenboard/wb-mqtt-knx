#pragma once

#include "datapointid.h"
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
            /// \param id a datapoint id
            /// \return DataPoint name
            std::string GetDataPointNameById(const knx::object::TDatapointId& id);
        }
    }
}
