#pragma once

#include "datapointid.h"
#include "idpt.h"
#include "idptbuilder.h"

namespace knx
{
    namespace object
    {
        class TDptWbMqttBuilder: public IDptBuilder
        {
        public:
            std::experimental::optional<PDpt> Create(const TDatapointId& datapointId) override;
            bool HasDpt(const TDatapointId& datapointId) const override;
            std::experimental::optional<std::string> GetDptConfigName(const TDatapointId& datapointId) const override;

            const TDatapointId DefaultDatapointId{0};
        };
    }
}
