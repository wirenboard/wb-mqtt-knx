#include "dptwbmqttconfig.h"
#include <map>
#include <vector>

using namespace knx::object;

namespace
{
    const std::vector<std::string> DptsCatalog = {"Raw_Value", // Default value
                                                  "1.xxx_B1",
                                                  "2.xxx_B2",
                                                  "3.xxx_B1U3",
                                                  "4.xxx_Character_Set",
                                                  "5.xxx_8-Bit_Unsigned_Value",
                                                  "6.xxx_V8",
                                                  "7.xxx_2-Octet_Unsigned_Value",
                                                  "8.xxx_2-Octet_Signed_Value",
                                                  "9.xxx_2-Octet_Float_Value",
                                                  "12.xxx_4-Octet_Unsigned_Value",
                                                  "13.xxx_4-Octet_Signed_Value",
                                                  "14.xxx_4-Octet_Float_Value",
                                                  "20.xxx_N8"};
}

const std::map<TDatapointId, std::string>& TDptWbMqttConfig::GetConfigNameMap() const
{
    return DatapointConfigName;
}

TDptWbMqttConfig::TDptWbMqttConfig()
{
    for (const auto& dptConfigString: DptsCatalog) {
        TDatapointId id{};
        id.SetFromString(dptConfigString);
        DatapointConfigName[id] = dptConfigString;
    }
}
