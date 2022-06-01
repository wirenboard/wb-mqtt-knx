#include "mqttbuilder.h"
#include "datapointpool.h"
#include "mqtt.h"
#include <regex>

using namespace knx::object;

TGroupObjectMqttBuilder::TGroupObjectMqttBuilder(WBMQTT::PDeviceDriver pMqttDeviceDriver,
                                                 object::IDptBuilder& dptJsonBuilder,
                                                 WBMQTT::TLogger& errorLogger)
    : MqttDeviceDriver(std::move(pMqttDeviceDriver)),
      DptJsonBuilder(dptJsonBuilder),
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

PGroupObject TGroupObjectMqttBuilder::Create(const TGroupObjectSettings& settings)
{
    if (MqttDeviceList.empty())
        return nullptr;

    PDpt datapoint;

    std::regex hasJsonRegex("_JSON\\s*$");
    if (std::regex_search(settings.DatapointType, hasJsonRegex)) {
        TDatapointId datapointId;
        if (!datapointId.SetFromString(settings.DatapointType)) {
            return nullptr;
        }
        datapoint = DptJsonBuilder.Create(datapointId);
        if (datapoint == nullptr) {
            return nullptr;
        }
    } else {
        datapoint = DataPointPool::MakeDataPointByName(settings.DatapointType);
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
