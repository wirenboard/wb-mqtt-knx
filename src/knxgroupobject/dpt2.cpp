#include "dpt2.h"

using namespace knx::object;

TDpt2::TDpt2(const TGroupObjectMqttParameter& parameter, const std::shared_ptr<mqtt::IMqttDeviceAdapter>& pMqttDevice)
    : TGroupObjectMqttBase(parameter.KnxAddress, pMqttDevice)
{
    ControlC = pMqttDevice->CreateControl(parameter.ControlId + "_c", "switch", parameter.ControlTitle, 0, 1);
    ControlV = pMqttDevice->CreateControl(parameter.ControlId + "_v", "switch", parameter.ControlTitle, 0, 1);

    ControlC->SetEventHandler([this](const WBMQTT::TAny& value) { MqttControlCNotify(value); });
    ControlV->SetEventHandler([this](const WBMQTT::TAny& value) { MqttControlVNotify(value); });
}

TDpt2::~TDpt2()
{}

void TDpt2::KnxNotify(const std::vector<uint8_t>& payload)
{
    FieldV = payload[0] & 0x01;
    FieldC = payload[0] & (1 << 1);
    ControlC->Send(FieldC);
    ControlV->Send(FieldV);
}

void TDpt2::MqttControlVNotify(const WBMQTT::TAny& value)
{
    FieldV = value.As<bool>();
    Send();
}

void TDpt2::MqttControlCNotify(const WBMQTT::TAny& value)
{
    FieldC = value.As<bool>();
    Send();
}

void TDpt2::Send()
{
    auto val = (FieldV ? 0x01 : 0x00) | (FieldC ? (1 << 1) : 0x00);
    KnxSend(telegram::TApci::GroupValueWrite, {static_cast<uint8_t>(val)});
}
