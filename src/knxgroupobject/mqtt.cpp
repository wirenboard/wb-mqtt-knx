#include "mqtt.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include <utility>

using namespace knx::object;

TGroupObjectMqtt::TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                   const std::string& controlId,
                                   const std::string& controlName,
                                   bool isReadOnly,
                                   std::shared_ptr<WBMQTT::TLocalDevice> pMqttDevice)
    : Dpt(std::move(pDpt)),
      MqttLocalDevice(std::move(pMqttDevice))
{
    auto descriptorList = Dpt->getDescriptor();
    uint32_t index = 0;

    for (const auto& fieldDescriptor: descriptorList) {
        auto control = MqttLocalDevice
                           ->CreateControl(MqttLocalDevice->GetDriver()->BeginTx(),
                                           WBMQTT::TControlArgs{}
                                               .SetId(controlId + "_" + fieldDescriptor.Name)
                                               .SetTitle(controlName)
                                               .SetType(fieldDescriptor.Type)
                                               .SetMin(fieldDescriptor.Min)
                                               .SetMax(fieldDescriptor.Max)
                                               .SetReadonly(isReadOnly))
                           .GetValue();
        control->SetOnValueReceiveHandler([index, this](const WBMQTT::PControl&,
                                                        const WBMQTT::TAny& value,
                                                        const WBMQTT::PDriverTx& tx) { MqttNotify(index, value); });
        ControlList.push_back(control);
        ++index;
    }
}

void TGroupObjectMqtt::MqttNotify(uint32_t index, const WBMQTT::TAny& value)
{
    std::vector<uint8_t> data;

    {
        std::lock_guard<std::mutex> lg(DptExchangeMutex);

        if (!Dpt->FromMqtt(index, value)) {
            return;
        }
        data = Dpt->ToKnx();
    }

    KnxSender->Send({SelfKnxAddress, telegram::TApci::GroupValueWrite, data});
}

void TGroupObjectMqtt::KnxNotify(const TGroupObjectTransaction& transaction)
{
    if ((transaction.Apci == telegram::TApci::GroupValueWrite) ||
        (transaction.Apci == telegram::TApci::GroupValueResponse)) {
        std::vector<WBMQTT::TAny> mqttData;

        {
            std::lock_guard<std::mutex> lg(DptExchangeMutex);

            if (!Dpt->FromKnx(transaction.Payload)) {
                return;
            }
            mqttData = Dpt->ToMqtt();
        }

        uint32_t index = 0;

        auto tx = MqttLocalDevice->GetDriver()->BeginTx();
        for (const auto& control: ControlList) {
            control->SetValue(tx, std::move(mqttData.at(index))).Wait();
            ++index;
        }
        tx->End();
    }
}

void TGroupObjectMqtt::SetKnxSender(const knx::TKnxGroupAddress& groupAddress,
                                    std::shared_ptr<ISenderGroupObject> sender)
{
    SelfKnxAddress = groupAddress;
    KnxSender = std::move(sender);
}
