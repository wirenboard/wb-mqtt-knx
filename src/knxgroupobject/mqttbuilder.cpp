#include "mqttbuilder.h"
#include "datapointpool.h"
#include "mqtt.h"

using namespace knx::object;

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

    return std::make_shared<knx::object::TGroupObjectMqtt>(DataPointPool::MakeDataPointByName(parameter.Type),
                                                           parameter.ControlId,
                                                           parameter.ControlTitle,
                                                           parameter.isReadOnly,
                                                           MqttDeviceList.back(),
                                                           ErrorLogger);
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
