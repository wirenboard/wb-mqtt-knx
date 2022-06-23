#pragma once
#include "basedptconfig.h"
#include <map>

namespace knx
{
    namespace object
    {
        class TDptJsonConfig: public TBaseDptConfig
        {
        public:
            explicit TDptJsonConfig(const std::string& descriptorPath);

        private:
            const std::map<TDatapointId, std::string>& GetConfigNameMap() const override;
            std::map<TDatapointId, std::string> DatapointNameMap;
        };
    } // knx
} // object
