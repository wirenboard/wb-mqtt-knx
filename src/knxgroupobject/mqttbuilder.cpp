#include "mqttbuilder.h"
#include "dptcommonconfig.h"
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
      DptWbMqttBuilder(dptWbMqttBuilder),
      ErrorLogger(errorLogger)
{}

void TGroupObjectMqttBuilder::LinkDevice(const std::string& id, const std::string& name)
{
    MqttDeviceList.push_back(
        MqttDeviceDriver->BeginTx()
            ->CreateDevice(
                WBMQTT::TLocalDeviceArgs{}.SetId(id).SetTitle(name).SetIsVirtual(true).SetDoLoadPrevious(true))
            .GetValue());
}

PGroupObject TGroupObjectMqttBuilder::Create(const TGroupObjectSettings& settings)
{
    if (MqttDeviceList.empty())
        return nullptr;

    PDpt datapoint;
    TDatapointId datapointId;
    if (settings.DatapointType == dptcommonconfig::RawValueString) {
        datapointId = dptcommonconfig::RawValueId;
    } else if (!datapointId.SetFromString(settings.DatapointType)) {
        return nullptr;
    }

    std::regex hasJsonRegex("_JSON\\s*$");
    if (std::regex_search(settings.DatapointType, hasJsonRegex)) {
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

    return std::make_shared<knx::object::TGroupObjectMqtt>(datapoint, settings, MqttDeviceList.back(), ErrorLogger);
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
