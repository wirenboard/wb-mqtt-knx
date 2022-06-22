#pragma once

#include "datapointid.h"
#include "idpt.h"
#include "idptbuilder.h"
#include <map>

namespace knx
{
    namespace object
    {
        class TDptWbMqttBuilder: public IDptBuilder
        {
        public:
            explicit TDptWbMqttBuilder();
            std::experimental::optional<PDpt> Create(const TDatapointId& datapointId) override;
            bool HasDpt(const TDatapointId& datapointId) const override;
            std::experimental::optional<std::string> GetDptConfigName(const TDatapointId& datapointId) const override;

            static const TDatapointId DefaultDatapointId;

        private:
            typedef PDpt (*TDptInstanceCreator)();

            std::map<TDatapointId, PDpt (*)()> DptInstanceCreatorMap;
            TDptInstanceCreator FindDptInstanceCreator(const TDatapointId& datapointId);
        };
    }
}
