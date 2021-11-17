#include "mqtt.h"

#include <utility>

using namespace knx::object;

TGroupObjectMqtt::TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                   const std::string& controlId,
                                   const std::string& controlName,
                                   std::shared_ptr<mqtt::IMqttDeviceAdapter> pMqttDevice)
    : Dpt(std::move(pDpt)),
      MqttDeviceAdapter(std::move(pMqttDevice))
{
    auto descriptorList = Dpt->getDescriptor();
    uint32_t index = 0;

    for (const auto& fieldDescriptor: descriptorList) {
        auto control = MqttDeviceAdapter->CreateControl(controlId + "_" + fieldDescriptor.Name,
                                                        fieldDescriptor.Type,
                                                        controlName,
                                                        fieldDescriptor.min,
                                                        fieldDescriptor.max);
        control->SetEventHandler([index, this](const WBMQTT::TAny& value) { MqttNotify(index, value); });
        ControlList.push_back(control);
        ++index;
    }
}

void TGroupObjectMqtt::KnxNotify(const std::vector<uint8_t>& data)
{
    auto mqttData = Dpt->knxToMqtt(data);
    uint32_t index = 0;
    for (const auto& control: ControlList) {
        control->Send(std::move(mqttData[index]));
        ++index;
    }
}

void TGroupObjectMqtt::MqttNotify(uint32_t index, const WBMQTT::TAny& value)
{
    auto data = Dpt->mqttToKnx(index, value);
    if (!data.empty())
        KnxSend(data);
}
