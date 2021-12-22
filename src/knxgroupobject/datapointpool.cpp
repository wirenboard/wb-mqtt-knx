#include "datapointpool.h"
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
        uint32_t GeneralGroupId;
        bool HasSubGroup{true};
        uint32_t SubGroupId{0};
    };

    const std::vector<TDptsCatalogItem> DptsCatalog = {
        {"Raw_Value", &CreateInst<TDptRaw>, 0}, // Default value
        {"1.xxx_B1", &CreateInst<TDpt1>, 1},
        {"2.xxx_B2", &CreateInst<TDpt2>, 2},
        {"3.xxx_B1U3", &CreateInst<TDpt3>, 3},
        {"4.xxx_Character_Set", &CreateInst<TDpt4>, 4},
        {"5.xxx_8-Bit_Unsigned_Value", &CreateInst<TDpt5>, 5},
        {"6.xxx_V8", &CreateInst<TDpt6>, 6},
        {"7.xxx_2-Octet_Unsigned_Value", &CreateInst<TDpt7>, 7},
        {"8.xxx_2-Octet_Signed_Value", &CreateInst<TDpt8>, 8},
        {"9.xxx_2-Octet_Float_Value", &CreateInst<TDpt9>, 9},
        {"12.001_4-Octet_Unsigned_Value", &CreateInst<TDpt12>, 12, true, 1},
        {"13.xxx_4-Octet_Signed_Value", &CreateInst<TDpt13>, 13},
        {"14.xxx_4-Octet_Float_Value", &CreateInst<TDpt14>, 14},
        {"20.xxx_N8", &CreateInst<TDpt20>, 20}};

    // TODO Add Dpts
    //        "6.020_Status_with_Mode",
    //        "10.001_Time",
    //        "11.001_Date",
    //        "15.000_Access_Data",
    //        "16.xxx_String",
    //        "17.xxx_Scene_Number",
    //        "18.001_Scene_Control",
    //        "19.001_DateTime",
}

PDpt DataPointPool::MakeDataPointByName(const std::string& name)
{
    auto it = std::find_if(DptsCatalog.begin(), DptsCatalog.end(), [name](const TDptsCatalogItem& item) {
        return (item.Name == name);
    });

    if (it == DptsCatalog.end())
        return DptsCatalog[DEFAULT_DPT_INDEX].Instance();

    return it->Instance();
}

std::string DataPointPool::GetDataPointNameById(uint32_t general, uint32_t sub)
{
    auto it = std::find_if(DptsCatalog.begin(), DptsCatalog.end(), [general, sub](const TDptsCatalogItem& item) {
        return (item.HasSubGroup) && (item.GeneralGroupId == general) && (item.SubGroupId == sub);
    });

    if (it == DptsCatalog.end()) {
        it = std::find_if(DptsCatalog.begin(), DptsCatalog.end(), [general](const TDptsCatalogItem& item) {
            return (!item.HasSubGroup) && (item.GeneralGroupId == general);
        });

        if (it == DptsCatalog.end()) {
            return DptsCatalog[DEFAULT_DPT_INDEX].Name;
        }
    }
    return it->Name;
}
