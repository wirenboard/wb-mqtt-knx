#include "mqttbuilder.h"
#include "dpt1.h"
#include "dpt12.h"
#include "dpt13.h"
#include "dpt14.h"
#include "dpt2.h"
#include "dpt3.h"
#include "dpt4.h"
#include "dpt5.h"
#include "dpt6.h"
#include "dpt7.h"
#include "dpt8.h"
#include "dpt9.h"
#include "dptraw.h"
#include "mqtt.h"

using namespace knx::object;

namespace
{
    template<typename T> PDpt CreateInst()
    {
        return std::make_shared<T>();
    }
}

TGroupObjectMqttBuilder::TGroupObjectMqttBuilder(WBMQTT::PDeviceDriver pMqttDeviceDriver, WBMQTT::TLogger& errorLogger)
    : MqttDeviceDriver(std::move(pMqttDeviceDriver)),
      ErrorLogger(errorLogger)
{}

void TGroupObjectMqttBuilder::LinkDevice(const std::string& id, const std::string& name)
{
    MqttDeviceList.push_back(
        MqttDeviceDriver->BeginTx()
            ->CreateDevice(
                WBMQTT::TLocalDeviceArgs{}.SetId(id).SetTitle(name).SetIsVirtual(true).SetDoLoadPrevious(false))
            .GetValue());
}

PGroupObject TGroupObjectMqttBuilder::Create(const TGroupObjectMqttParameter& parameter)
{
    if (MqttDeviceList.empty())
        return nullptr;

    // TODO Add Dpts
    static const std::unordered_map<std::string, PDpt (*)()> dptsMap = {
        {"Raw_Value", &CreateInst<TDptRaw>},
        {"1.xxx_B1", &CreateInst<TDpt1>},
        {"2.xxx_B2", &CreateInst<TDpt2>},
        {"3.xxx_B1U3", &CreateInst<TDpt3>},
        {"4.xxx_Character_Set", &CreateInst<TDpt4>},
        {"5.xxx_8-Bit_Unsigned_Value", &CreateInst<TDpt5>},
        {"6.xxx_V8", &CreateInst<TDpt6>},
        {"7.xxx_2-Octet_Unsigned_Value", &CreateInst<TDpt7>},
        {"8.xxx_2-Octet_Signed_Value", &CreateInst<TDpt8>},
        {"9.xxx_2-Octet_Float_Value", &CreateInst<TDpt9>},
        {"12.001_4-Octet_Unsigned_Value", &CreateInst<TDpt12>},
        {"13.xxx_4-Octet_Signed_Value", &CreateInst<TDpt13>},
        {"14.xxx_4-Octet_Float_Value", &CreateInst<TDpt14>}};

    auto it = dptsMap.find(parameter.Type);
    if (it == dptsMap.end())
        return nullptr;

    return std::make_shared<knx::object::TGroupObjectMqtt>(it->second(),
                                                           parameter.ControlId,
                                                           parameter.ControlTitle,
                                                           parameter.isReadOnly,
                                                           MqttDeviceList.back(),
                                                           ErrorLogger);
    //        "6.020_Status_with_Mode",
    //        "10.001_Time",
    //        "11.001_Date",
    //        "15.000_Access_Data",
    //        "16.xxx_String",
    //        "17.xxx_Scene_Number",
    //        "18.001_Scene_Control",
    //        "19.001_DateTime",
}

void TGroupObjectMqttBuilder::RemoveUnusedControls()
{
    if (!MqttDeviceList.empty()) {
        MqttDeviceList.back()->RemoveUnusedControls(MqttDeviceList.back()->GetDriver()->BeginTx()).Wait();
    }
}

void TGroupObjectMqttBuilder::Clear()
{
    auto tx = MqttDeviceDriver->BeginTx();
    for (const auto& device: MqttDeviceList) {
        tx->RemoveDeviceById(device->GetId()).Wait();
    }
}
