#include "dpt1.h"

using namespace knx::object;

TDpt1::TDpt1(const TGroupObjectMqttParameter& parameter, const std::shared_ptr<mqtt::IMqttDeviceAdapter>& pMqttDevice)
    : TGroupObjectMqttBase(pMqttDevice)
{
    ControlB = pMqttDevice->CreateControl(parameter.ControlId + "_b", "switch", parameter.ControlTitle, 0, 1);
    ControlB->SetEventHandler([this](const WBMQTT::TAny& value) { MqttControlBNotify(value); });
}

TDpt1::~TDpt1()
{}

void TDpt1::KnxNotify(const std::vector<uint8_t>& payload)
{
    B = payload[0] & 0x01;
    ControlB->Send(B);
}

void TDpt1::MqttControlBNotify(const WBMQTT::TAny& value)
{
    B = value.As<bool>();
    auto val = B ? 0x01 : 0x00;
    KnxSend({static_cast<uint8_t>(val)});
}
