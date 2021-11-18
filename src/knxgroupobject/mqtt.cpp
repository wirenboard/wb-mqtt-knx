#include "mqtt.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
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

void TGroupObjectMqtt::MqttNotify(uint32_t index, const WBMQTT::TAny& value)
{
    if (Dpt->FromMqtt(index, value)) {
        KnxSender->Send({SelfKnxAddress, telegram::TApci::GroupValueWrite, Dpt->ToKnx()});
    }
}

void TGroupObjectMqtt::KnxNotify(const TGroupObjectTransaction& transaction)
{
    if (((transaction.Apci == telegram::TApci::GroupValueWrite) ||
         (transaction.Apci == telegram::TApci::GroupValueResponse)) &&
        Dpt->FromKnx(transaction.Payload))
    {
        auto mqttData = Dpt->ToMqtt();
        uint32_t index = 0;
        for (const auto& control: ControlList) {
            control->Send(std::move(mqttData.at(index)));
            ++index;
        }
    }
}

void TGroupObjectMqtt::SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                    std::shared_ptr<ISenderGroupObject> sender)
{
    SelfKnxAddress = groupAddress;
    KnxSender = std::move(sender);
}
