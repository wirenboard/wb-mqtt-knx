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
            const TConfigNameMap& GetConfigNameMap() const override;
            TConfigNameMap DatapointNameMap;
        };
    } // knx
} // object
