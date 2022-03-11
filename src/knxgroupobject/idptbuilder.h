#pragma once
#include "datapointid.h"
#include "idpt.h"

namespace knx
{
    namespace object
    {
        class IDptBuilder
        {
        public:
            virtual PDpt Create(const TDatapointId& datapointId) = 0;
        };
    }
}