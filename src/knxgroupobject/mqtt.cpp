#include "mqtt.h"
#include "../knxgroupaddress.h"
#include "../knxtelegram.h"
#include <utility>

using namespace knx::object;

TGroupObjectMqtt::TGroupObjectMqtt(PDpt pDpt,
                                   const knx::TGroupObjectSettings& settings,
                                   WBMQTT::PLocalDevice pMqttDevice,
                                   WBMQTT::TLogger& errorLogger)
    : Dpt(std::move(pDpt)),
      Settings(settings),
      MqttLocalDevice(std::move(pMqttDevice)),
      ErrorLogger(errorLogger)
{
    auto descriptorList = Dpt->getDescriptor();
    uint32_t index = 0;

    auto tx = MqttLocalDevice->GetDriver()->BeginTx();

    for (const auto& fieldDescriptor: descriptorList) {
        std::string fullControlId;
        std::string fullControlTitle;
        if (descriptorList.size() == 1) {
            fullControlId = settings.ControlId;
            fullControlTitle = settings.ControlTitle;
        } else {
            fullControlId = settings.ControlId + "_" + fieldDescriptor.Id;
            fullControlTitle = settings.ControlTitle + " ." + fieldDescriptor.Id;
        }
        auto control = MqttLocalDevice
                           ->CreateControl(tx,
                                           WBMQTT::TControlArgs{}
                                               .SetId(fullControlId)
                                               .SetTitle(fullControlTitle)
                                               .SetType(fieldDescriptor.Type)
                                               .SetMin(fieldDescriptor.Min)
                                               .SetMax(fieldDescriptor.Max)
                                               .SetReadonly(settings.IsReadOnly))
                           .GetValue();
        control->SetOnValueReceiveHandler(
            [index, this](WBMQTT::PControl pControl, const WBMQTT::TAny& value, const WBMQTT::PDriverTx&) {
                MqttNotify(pControl, index, value);
            });
        ControlList.push_back(control);
        ++index;
    }
}

void TGroupObjectMqtt::MqttNotify(WBMQTT::PControl& pControl, uint32_t index, const WBMQTT::TAny& value)
{
    std::vector<uint8_t> data;
    auto tx = MqttLocalDevice->GetDriver()->CreateUnsafeTx();

    {
        std::lock_guard<std::mutex> lg(DptExchangeMutex);
        try {
            Dpt->FromMqtt(index, value);
            data = Dpt->ToKnx();
        } catch (const std::exception& exception) {
            auto setErrorFuture = pControl->SetError(tx, exception.what());
            ErrorLogger.Log() << "Invalid Mqtt Control value: "
                              << MqttLocalDevice->GetId() + "." + pControl->GetId() + " : " << exception.what();
            setErrorFuture.Wait();
            return;
        }
    }

    KnxSender->Send({SelfKnxAddress, telegram::TApci::GroupValueWrite, data});

    if (!Settings.FeedbackGroupAddress) {
        pControl->SetValue(tx, WBMQTT::TAny(value)).Wait();
    }
}

void TGroupObjectMqtt::KnxNotify(const TGroupObjectTransaction& transaction)
{
    if ((transaction.Apci == telegram::TApci::GroupValueWrite) ||
        (transaction.Apci == telegram::TApci::GroupValueResponse)) {
        std::vector<WBMQTT::TAny> mqttData;

        {
            std::lock_guard<std::mutex> lg(DptExchangeMutex);

            try {
                Dpt->FromKnx(transaction.Payload);
                mqttData = Dpt->ToMqtt();
            } catch (const std::exception& exception) {
                ErrorLogger.Log() << "Invalid Dpt format (" + transaction.Address.ToString() + ") : "
                                  << exception.what();

                auto tx = MqttLocalDevice->GetDriver()->BeginTx();
                for (const auto& control: ControlList) {
                    control->SetError(tx, exception.what()).Wait();
                }

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

void TGroupObjectMqtt::SetKnxSender(const knx::TKnxGroupAddress& groupAddress, PSenderGroupObject sender)
{
    SelfKnxAddress = groupAddress;
    KnxSender = std::move(sender);
}

void TGroupObjectMqtt::KnxNotifyEvent(const TKnxEvent& event)
{
    std::string errorMessage;
    switch (event) {
        case TKnxEvent::None:
        case TKnxEvent::ReceivedTelegram:
            return;
        case TKnxEvent::KnxdSocketConnected:
            errorMessage = "";
            break;
        case TKnxEvent::PollReadTimeoutError:
            errorMessage = "Timed out waiting for a response to a read request";
            break;
        case TKnxEvent::KnxdSocketError:
            errorMessage = "knxd socket error";
    }

    auto tx = MqttLocalDevice->GetDriver()->BeginTx();
    for (const auto& control: ControlList) {
        control->SetError(tx, errorMessage).Wait();
    }
}
