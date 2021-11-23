#include "mqtt.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include <utility>

using namespace knx::object;

TGroupObjectMqtt::TGroupObjectMqtt(std::shared_ptr<IDpt> pDpt,
                                   const std::string& controlId,
                                   const std::string& controlName,
                                   bool isReadOnly,
                                   std::shared_ptr<WBMQTT::TLocalDevice> pMqttDevice,
                                   WBMQTT::TLogger& errorLogger)
    : Dpt(std::move(pDpt)),
      MqttLocalDevice(std::move(pMqttDevice)),
      ErrorLogger(errorLogger)
{
    auto descriptorList = Dpt->getDescriptor();
    uint32_t index = 0;

    auto tx = MqttLocalDevice->GetDriver()->BeginTx();

    for (const auto& fieldDescriptor: descriptorList) {
        auto fullControlId = controlId + "_" + fieldDescriptor.Name;
        auto control = MqttLocalDevice
                           ->CreateControl(tx,
                                           WBMQTT::TControlArgs{}
                                               .SetId(fullControlId)
                                               .SetTitle(controlName)
                                               .SetType(fieldDescriptor.Type)
                                               .SetMin(fieldDescriptor.Min)
                                               .SetMax(fieldDescriptor.Max)
                                               .SetReadonly(isReadOnly))
                           .GetValue();
        control->SetOnValueReceiveHandler([index, fullControlId, this](const WBMQTT::PControl&,
                                                                       const WBMQTT::TAny& value,
                                                                       const WBMQTT::PDriverTx& tx) {
            MqttNotify(MqttLocalDevice->GetId(), fullControlId, index, value);
        });
        ControlList.push_back(control);
        ++index;
    }
}

void TGroupObjectMqtt::MqttNotify(const std::string& deviceId,
                                  const std::string& controlId,
                                  uint32_t index,
                                  const WBMQTT::TAny& value)
{
    std::vector<uint8_t> data;

    {
        std::lock_guard<std::mutex> lg(DptExchangeMutex);
        try {
            if (!Dpt->FromMqtt(index, value)) {
                return;
            }
            data = Dpt->ToKnx();
        } catch (const std::exception& exception) {
            ErrorLogger.Log() << "Invalid Mqtt Control value: " << deviceId + "." + controlId + " : "
                              << exception.what();
            return;
        }
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

            try {
                if (!Dpt->FromKnx(transaction.Payload)) {
                    return;
                }
                mqttData = Dpt->ToMqtt();
            } catch (const std::exception& exception) {
                ErrorLogger.Log() << "Invalid Dpt format (" + transaction.Address.ToString() + ") : "
                                  << exception.what();
                return;
            }
        }

        uint32_t index = 0;

        auto tx = MqttLocalDevice->GetDriver()->BeginTx();
        for (const auto& control: ControlList) {
            control->SetValue(tx, std::move(mqttData.at(index))).Wait();
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
