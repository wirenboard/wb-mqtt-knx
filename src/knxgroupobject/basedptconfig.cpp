#include "basedptconfig.h"

using namespace knx::object;

std::experimental::optional<std::string> TBaseDptConfig::GetDptConfigName(const TDatapointId& datapointId) const
{
    auto DatapointConfigName = GetConfigNameMap();
    auto id = datapointId;
    if (datapointId.HasSubId()) {
        auto it = DatapointConfigName.find(id);
        if (it != DatapointConfigName.end()) {
            return it->second;
        }
        id.ClearSub();
    }

    auto it = DatapointConfigName.find(id);
    if (it != DatapointConfigName.end()) {
        return it->second;
    }

    return std::experimental::nullopt;
}