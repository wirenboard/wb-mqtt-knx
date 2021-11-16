#include "mqttbuilder.h"
#include "dpt1.h"
#include "dpt2.h"

using namespace knx::object;

TGroupObjectMqttBuilder::TGroupObjectMqttBuilder(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDeviceDriver)
    : MqttDeviceDriver(std::move(pMqttDeviceDriver))
{}

void TGroupObjectMqttBuilder::LinkDevice(const std::string& id, const std::string& name)
{
    auto localDevice =
        MqttDeviceDriver->BeginTx()
            ->CreateDevice(
                WBMQTT::TLocalDeviceArgs{}.SetId(id).SetTitle(name).SetIsVirtual(true).SetDoLoadPrevious(false))
            .GetValue();
    MqttDevice = std::make_shared<mqtt::MqttDeviceAdapter>(localDevice);
}

std::shared_ptr<TGroupObjectMqttBase> TGroupObjectMqttBuilder::Create(const TGroupObjectMqttParameter& parameter)
{
    // TODO
    if (parameter.Type == "1.xxx_B1") {
        return std::make_shared<knx::object::TDpt1>(parameter, MqttDevice);
    } else if (parameter.Type == "2.xxx_B2") {
        return std::make_shared<knx::object::TDpt2>(parameter, MqttDevice);
    }
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
    //    return std::make_shared<knx::object::TDpt1>(parameter, MqttDevice);
    return nullptr;
}
