#include "mqttbuilder.h"
#include "dpt1.h"

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
    return std::make_shared<knx::object::TDpt1>(parameter, MqttDevice);
}
