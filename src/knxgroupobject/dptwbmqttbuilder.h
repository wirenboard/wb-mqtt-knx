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
            PDpt Create(const TDatapointId& datapointId) override;

            static const TDatapointId DefaultDatapointId;

        private:
            typedef PDpt (*TDptInstanceCreator)();

            std::map<TDatapointId, PDpt (*)()> DptInstanceCreatorMap;
            TDptInstanceCreator FindDptInstanceCreator(const TDatapointId& datapointId);
        };
    }
}
