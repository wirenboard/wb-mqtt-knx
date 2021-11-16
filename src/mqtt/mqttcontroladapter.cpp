#include "mqttcontroladapter.h"

using namespace mqtt;

MqttControlAdapter::MqttControlAdapter(std::shared_ptr<WBMQTT::TControl> control): Control(std::move(control))
{}

void MqttControlAdapter::Send(WBMQTT::TAny&& value)
{
    auto tx = Control->GetDevice()->GetDriver()->BeginTx();
    Control->SetValue(tx, std::move(value)).Wait();
    tx->End();
}

void MqttControlAdapter::SetEventHandler(const std::function<void(const WBMQTT::TAny&)>& f)
{
    Control->SetOnValueReceiveHandler(
        [f](WBMQTT::PControl, const WBMQTT::TAny& value, const WBMQTT::PDriverTx& tx) { f(value); });
}
