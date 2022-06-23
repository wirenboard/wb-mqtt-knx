#pragma once
#include "datapointid.h"
#include <experimental/optional>
#include <map>

namespace knx
{
    namespace object
    {
        class TBaseDptConfig
        {
        public:
            /// Get a datapoint config name from Id
            /// \param datapointId datapoint Id
            /// \return optional datapoint name. Or std::nullopt if datapoint not found
            std::experimental::optional<std::string> GetDptConfigName(const TDatapointId& datapointId) const;

            static const TDatapointId DefaultDatapointId;

            /// Virtual destructor
            virtual ~TBaseDptConfig() = default;

        protected:
            virtual const std::map<TDatapointId, std::string>& GetConfigNameMap() const = 0;
        };

    } // knx
} // object
