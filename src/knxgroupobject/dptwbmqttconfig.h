#pragma once
#include "basedptconfig.h"
#include <map>

namespace knx
{
    namespace object
    {
        class TDptWbMqttConfig: public TBaseDptConfig
        {
        public:
            explicit TDptWbMqttConfig();

        private:
            const TConfigNameMap& GetConfigNameMap() const override;

            TConfigNameMap DatapointConfigName;
        };
    }
}
