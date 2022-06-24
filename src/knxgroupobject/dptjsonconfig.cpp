#include "dptjsonconfig.h"
#include <wblib/json_utils.h>

using namespace knx::object;

TDptJsonConfig::TDptJsonConfig(const std::string& descriptorPath)
{
    auto descriptorRoot = WBMQTT::JSON::Parse(descriptorPath);

    for (const auto& descriptor: descriptorRoot) {
        auto mainId = descriptor["id"].asUInt();

        const TDatapointId id{mainId};
        DatapointNameMap[id] = id.ToString() + "_" + descriptor["name"].asString() + "_JSON";

        for (const auto& datapoint: descriptor["datapoint"]) {
            auto datapointId = id;
            datapointId.SetSub(datapoint["subid"].asUInt());
            DatapointNameMap[datapointId] = datapointId.ToString() + "_" + datapoint["name"].asString() + "_JSON";
        }
    }
}

const TBaseDptConfig::TConfigNameMap& TDptJsonConfig::GetConfigNameMap() const
{
    return DatapointNameMap;
}
