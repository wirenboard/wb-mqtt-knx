#include "mqttknxconfig.h"
#include <fstream>

using namespace knx::tool;

void TMqttKnxConfig::AddHeader(uint32_t configVersion, bool debug, bool enableLegacyKnxDevice)
{
    RootObject["configVersion"] = configVersion;
    RootObject["debug"] = debug;
    RootObject["devices"] = Json::Value(Json::arrayValue);
    RootObject["enableLegacyKnxDevice"] = enableLegacyKnxDevice;
}

void TMqttKnxConfig::AddDevice(const std::string& id,
                               const std::string& title,
                               const std::vector<TControlConfig>& controls)
{
    auto DeviceObjectValue = Json::Value(Json::objectValue);
    DeviceObjectValue["deviceId"] = id;
    DeviceObjectValue["deviceTitle"] = title;
    DeviceObjectValue["controls"] = Json::Value(Json::arrayValue);
    for (const auto& control: controls) {
        auto ControlObjectValue = Json::Value(Json::objectValue);
        ControlObjectValue["controlId"] = control.Id;
        ControlObjectValue["controlTitle"] = control.Title;
        ControlObjectValue["groupAddress"] = control.GroupAddress.ToString();
        ControlObjectValue["dataPointType"] = control.DatapointType;
        ControlObjectValue["readOnly"] = control.ReadOnly;
        DeviceObjectValue["controls"].append(ControlObjectValue);
    }

    if (RootObject["devices"].empty()) {
        RootObject["devices"] = Json::Value(Json::arrayValue);
    }
    RootObject["devices"].append(DeviceObjectValue);
}

void TMqttKnxConfig::SaveConfig(const std::string& path)
{
    std::ofstream fileStream;
    fileStream.open(path, std::fstream::out);

    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "  ";
    builder.settings_["emitUTF8"] = true;
    auto writer = std::unique_ptr<Json::StreamWriter>(builder.newStreamWriter());
    writer->write(RootObject, &fileStream);
}
