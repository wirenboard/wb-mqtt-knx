#include "mqttbuilder.h"
#include "dpt1.h"
#include "dpt2.h"
#include "dpt9.h"
#include "dptraw.h"
#include "mqtt.h"

using namespace knx::object;

namespace
{
    template<typename T> std::shared_ptr<IDpt> CreateInst()
    {
        return std::make_shared<T>();
    }
}

TGroupObjectMqttBuilder::TGroupObjectMqttBuilder(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDeviceDriver)
    : MqttDeviceDriver(std::move(pMqttDeviceDriver))
{}

void TGroupObjectMqttBuilder::LinkDevice(const std::string& id, const std::string& name)
{
    MqttDeviceList.push_back(
        MqttDeviceDriver->BeginTx()
            ->CreateDevice(
                WBMQTT::TLocalDeviceArgs{}.SetId(id).SetTitle(name).SetIsVirtual(true).SetDoLoadPrevious(false))
            .GetValue());
}

std::shared_ptr<IGroupObject> TGroupObjectMqttBuilder::Create(const TGroupObjectMqttParameter& parameter)
{
    if (MqttDeviceList.empty())
        return nullptr;

    // TODO Add Dpts
    static const std::unordered_map<std::string, std::shared_ptr<IDpt> (*)()> dptsMap = {
        {"Raw_Value", &CreateInst<TDptRaw>},
        {"1.xxx_B1", &CreateInst<TDpt1>},
        {"2.xxx_B2", &CreateInst<TDpt2>},
        {"9.xxx_2-Octet_Float_Value", &CreateInst<TDpt9>}};

    auto it = dptsMap.find(parameter.Type);
    if (it == dptsMap.end())
        return nullptr;

    return std::make_shared<knx::object::TGroupObjectMqtt>(it->second(),
                                                           parameter.ControlId,
                                                           parameter.ControlTitle,
                                                           parameter.isReadOnly,
                                                           MqttDeviceList.back());
    //        "2.xxx_B2",
    //        "3.xxx_B1U3",
    //        "4.xxx_Character_Set",
    //        "5.xxx_8-Bit_Unsigned_Value",
    //        "6.xxx_V8",
    //        "6.020_Status_with_Mode",
    //        "7.xxx_2-Octet_Unsigned_Value",
    //        "8.xxx_2-Octet_Signed_Value",
    //        "9.xxx_2-Octet_Float_Value",
    //        "10.001_Time",
    //        "11.001_Date",
    //        "12.001_4-Octet_Unsigned_Value",
    //        "13.xxx_4-Octet_Signed_Value",
    //        "14.xxx_4-Octet_Float_Value",
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
    tx->End();
}
