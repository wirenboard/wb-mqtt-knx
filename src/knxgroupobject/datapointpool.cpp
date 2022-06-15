#include "datapointpool.h"
#include "../knxexception.h"
#include "dpt1.h"
#include "dpt12.h"
#include "dpt13.h"
#include "dpt14.h"
#include "dpt2.h"
#include "dpt20.h"
#include "dpt3.h"
#include "dpt4.h"
#include "dpt5.h"
#include "dpt6.h"
#include "dpt7.h"
#include "dpt8.h"
#include "dpt9.h"
#include "dptraw.h"
#include "idpt.h"
#include <algorithm>
#include <experimental/optional>
#include <map>

using namespace knx::object;

namespace
{

    constexpr auto DEFAULT_DPT_INDEX = 0U;

    template<typename T> PDpt CreateInst()
    {
        return std::make_shared<T>();
    }

    struct TDptsCatalogItem
    {
        std::string Name;
        PDpt (*Instance)();
    };

    const std::map<TDatapointId, TDptsCatalogItem> DptsCatalog = {
        {TDatapointId{0}, {"Raw_Value", &CreateInst<TDptRaw>}}, // Default value
        {TDatapointId{1}, {"1.xxx_B1", &CreateInst<TDpt1>}},
        {TDatapointId{2}, {"2.xxx_B2", &CreateInst<TDpt2>}},
        {TDatapointId{3}, {"3.xxx_B1U3", &CreateInst<TDpt3>}},
        {TDatapointId{4}, {"4.xxx_Character_Set", &CreateInst<TDpt4>}},
        {TDatapointId{5}, {"5.xxx_8-Bit_Unsigned_Value", &CreateInst<TDpt5>}},
        {TDatapointId{6}, {"6.xxx_V8", &CreateInst<TDpt6>}},
        {TDatapointId{7}, {"7.xxx_2-Octet_Unsigned_Value", &CreateInst<TDpt7>}},
        {TDatapointId{8}, {"8.xxx_2-Octet_Signed_Value", &CreateInst<TDpt8>}},
        {TDatapointId{9}, {"9.xxx_2-Octet_Float_Value", &CreateInst<TDpt9>}},
        {TDatapointId{12}, {"12.xxx_4-Octet_Unsigned_Value", &CreateInst<TDpt12>}},
        {TDatapointId{12, 1}, {"12.001_4-Octet_Unsigned_Value", &CreateInst<TDpt12>}},
        {TDatapointId{13}, {"13.xxx_4-Octet_Signed_Value", &CreateInst<TDpt13>}},
        {TDatapointId{14}, {"14.xxx_4-Octet_Float_Value", &CreateInst<TDpt14>}},
        {TDatapointId{20}, {"20.xxx_N8", &CreateInst<TDpt20>}}};

    // TODO Add Dpts
    //        "6.020_Status_with_Mode",
    //        "10.001_Time",
    //        "11.001_Date",
    //        "15.000_Access_Data",
    //        "16.xxx_String",
    //        "17.xxx_Scene_Number",
    //        "18.001_Scene_Control",
    //        "19.001_DateTime",

    std::experimental::optional<TDptsCatalogItem> FindItem(const TDatapointId& datapointId)
    {
        auto id = datapointId;
        auto it = DptsCatalog.find(id);
        if (it != DptsCatalog.end()) {
            return it->second;
        }

        id.ClearSub();
        it = DptsCatalog.find(id);
        if (it != DptsCatalog.end()) {
            return it->second;
        }

        return {};
    }
}

std::experimental::optional<PDpt> TDptWbMqttBuilder::Create(const TDatapointId& datapointId)
{
    auto item = FindItem(datapointId);

    if (item) {
        return item->Instance();
    }

    wb_throw(TKnxException,
             "Can't create JSON datapoint id: " + datapointId.ToString() + ". There is no matching descriptor");
}

bool TDptWbMqttBuilder::HasDpt(const TDatapointId& datapointId) const
{
    auto item = FindItem(datapointId);

    return item ? true : false;
}

std::experimental::optional<std::string> TDptWbMqttBuilder::GetDptConfigName(const TDatapointId& datapointId) const
{
    auto item = FindItem(datapointId);
    if (item) {
        return item->Name;
    }
    return std::experimental::nullopt;
}
