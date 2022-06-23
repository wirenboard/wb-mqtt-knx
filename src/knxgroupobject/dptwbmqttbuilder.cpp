#include "dptwbmqttbuilder.h"
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

const TDatapointId TDptWbMqttBuilder::DefaultDatapointId{0};

namespace
{
    template<typename T> PDpt CreateInst()
    {
        return std::make_shared<T>();
    }

    const std::vector<PDpt (*)()> Dpts = {&CreateInst<TDptRaw>,
                                          &CreateInst<TDpt1>,
                                          &CreateInst<TDpt2>,
                                          &CreateInst<TDpt3>,
                                          &CreateInst<TDpt4>,
                                          &CreateInst<TDpt5>,
                                          &CreateInst<TDpt6>,
                                          &CreateInst<TDpt7>,
                                          &CreateInst<TDpt8>,
                                          &CreateInst<TDpt9>,
                                          &CreateInst<TDpt12>,
                                          &CreateInst<TDpt12>,
                                          &CreateInst<TDpt13>,
                                          &CreateInst<TDpt14>,
                                          &CreateInst<TDpt20>};

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

TDptWbMqttBuilder::TDptInstanceCreator TDptWbMqttBuilder::FindDptInstanceCreator(const TDatapointId& datapointId)
{
    auto id = datapointId;
    auto it = DptInstanceCreatorMap.find(id);
    if (it != DptInstanceCreatorMap.end()) {
        return it->second;
    }

    id.ClearSub();
    it = DptInstanceCreatorMap.find(id);
    if (it != DptInstanceCreatorMap.end()) {
        return it->second;
    }

    return nullptr;
}

PDpt TDptWbMqttBuilder::Create(const TDatapointId& datapointId)
{
    auto item = FindDptInstanceCreator(datapointId);

    if (item) {
        return item();
    }

    return nullptr;
}

TDptWbMqttBuilder::TDptWbMqttBuilder()
{
    for (const auto& dptInstanceCreator: Dpts) {
        auto dpt = dptInstanceCreator();
        DptInstanceCreatorMap[dpt->GetId()] = dptInstanceCreator;
    }
}
