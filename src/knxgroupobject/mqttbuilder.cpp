#include "mqttbuilder.h"
#include "dptwbmqttbuilder.h"
#include "mqtt.h"
#include <regex>

using namespace knx::object;

TGroupObjectMqttBuilder::TGroupObjectMqttBuilder(WBMQTT::PDeviceDriver pMqttDeviceDriver,
                                                 object::IDptBuilder& dptJsonBuilder,
                                                 object::IDptBuilder& dptWbMqttBuilder,
                                                 WBMQTT::TLogger& errorLogger)
    : MqttDeviceDriver(std::move(pMqttDeviceDriver)),
      DptJsonBuilder(dptJsonBuilder),
      DptWbMqttBuilder(dptJsonBuilder),
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

    PDpt datapoint;
    TDatapointId datapointId;
    if (!datapointId.SetFromString(parameter.Type)) {
        return nullptr;
    }

    std::regex hasJsonRegex("_JSON\\s*$");
    if (std::regex_search(parameter.Type, hasJsonRegex)) {
        datapoint = DptJsonBuilder.Create(datapointId);
        if (datapoint == nullptr) {
            wb_throw(TKnxException,
                     "Can't create JSON datapoint id: " + datapointId.ToString() +
                         ". There is no matching descriptor in the descriptor file.");
        }
    } else {
        datapoint = DptWbMqttBuilder.Create(datapointId);
        if (datapoint == nullptr) {
            wb_throw(TKnxException,
                     "Can't create WB-MQTT datapoint id: " + datapointId.ToString() +
                         ". Can not found Datapoint Name from Id");
        }
    }

    return std::make_shared<knx::object::TGroupObjectMqtt>(datapoint,
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
