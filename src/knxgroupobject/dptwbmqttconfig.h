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
            const std::map<TDatapointId, std::string>& GetConfigNameMap() const override;

            std::map<TDatapointId, std::string> DatapointConfigName;
        };
    }
}
