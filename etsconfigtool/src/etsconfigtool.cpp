#include "etsconfigtool.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

using namespace knx::tool;

namespace
{
    constexpr auto DEFAULT_CONFIG_VERSION = 1;
    constexpr auto DEFAULT_ENABLE_DEBUG = false;
    constexpr auto DEFAULT_ENABLE_LEGACY_KNX_DEVICE = false;
    constexpr auto DEFAULT_IS_CONTROL_READONLY = false;
}

TEtsConfigTool::TEtsConfigTool(const object::TBaseDptConfig& mqttConfig,
                               const object::TBaseDptConfig& jsonConfig,
                               const object::TDatapointId& defaultId)
    : MqttConfig(mqttConfig),
      JsonConfig(jsonConfig),
      DefaultId(defaultId)
{}

std::string TEtsConfigTool::DatapointTypeExportToConfig(const std::string& dpts)
{
    try {
        knx::object::TDatapointId id;
        auto firstDpts = WBMQTT::StringSplit(dpts, ",").at(0);
        auto firstDptsTokens = WBMQTT::StringSplit(firstDpts, "-");
        id.SetMain(std::stoi(firstDptsTokens.at(1)));
        if (firstDptsTokens.size() == 3) {
            id.SetSub(std::stoi(firstDptsTokens.at(2)));
        }

        auto datapoint = MqttConfig.GetDptConfigName(id);
        if (datapoint) {
            return *datapoint;
        }
        datapoint = JsonConfig.GetDptConfigName(id);
        if (datapoint) {
            return *datapoint;
        }
        return *MqttConfig.GetDptConfigName(DefaultId);
    } catch (const std::out_of_range& oor) {
        std::cerr << "DatapointTypeExportToConfig( " << dpts << " ): Out of Range error: " << oor.what() << '\n';
        return "";
    }
}

void TEtsConfigTool::AddToControlConfig(tinyxml2::XMLElement* groupAddress,
                                        std::vector<knx::tool::TControlConfig>& controlList)
{
    auto name = groupAddress->Attribute("Name");
    auto address = groupAddress->Attribute("Address");
    auto dpts = groupAddress->Attribute("DPTs");
    if ((name != nullptr) && (address != nullptr) && (dpts != nullptr)) {
        TControlConfig control;
        knx::TKnxGroupAddress knxGroupAddress(address);
        auto groupAddressStr = knxGroupAddress.ToString();
        std::replace(groupAddressStr.begin(), groupAddressStr.end(), '/', '_');
        control.Id = std::string("control") + groupAddressStr;
        control.Title = name;
        control.GroupAddress = knxGroupAddress;
        control.DatapointType = DatapointTypeExportToConfig(dpts);
        control.ReadOnly = DEFAULT_IS_CONTROL_READONLY;
        if (!control.DatapointType.empty()) {
            controlList.push_back(control);
        }
    }
}

void TEtsConfigTool::LoadEtsExport(const std::string& filePath)
{
    Config.AddHeader(DEFAULT_CONFIG_VERSION, DEFAULT_ENABLE_DEBUG, DEFAULT_ENABLE_LEGACY_KNX_DEVICE);

    tinyxml2::XMLDocument xmlDoc;
    auto result = xmlDoc.LoadFile(filePath.c_str());
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        wb_throw(TKnxException, "XMLDocument error: " + std::string{xmlDoc.ErrorName()});
    }

    auto root = xmlDoc.RootElement();

    if (root == nullptr) {
        wb_throw(TKnxException, "ETS export RootElement is corrupted");
    }

    for (auto mainGroup = root->FirstChildElement(); mainGroup != nullptr; mainGroup = mainGroup->NextSiblingElement())
    {
        for (auto middleObject = mainGroup->FirstChildElement(); middleObject != nullptr;
             middleObject = middleObject->NextSiblingElement())
        {
            std::string middleObjectStr = middleObject->Name();
            if (middleObjectStr == "GroupRange") {
                ParseGroupAddress(mainGroup, middleObject);
            } else if (middleObjectStr == "GroupAddress") {
                ParseGroupAddress(mainGroup);
                break;
            }
        }
    }
}

void TEtsConfigTool::SaveWbMqttConfig(const std::string& filePath)
{
    Config.SaveConfig(filePath);
}

void TEtsConfigTool::ParseGroupAddress(tinyxml2::XMLElement* mainGroup, tinyxml2::XMLElement* middleGroup)
{
    auto mainGroupName = mainGroup->Attribute("Name");
    auto middleGroupName = middleGroup->Attribute("Name");

    std::vector<knx::tool::TControlConfig> controlList;
    for (auto groupAddress = middleGroup->FirstChildElement(); groupAddress != nullptr;
         groupAddress = groupAddress->NextSiblingElement())
    {
        AddToControlConfig(groupAddress, controlList);
    }

    if (!controlList.empty()) {
        std::string deviceId = "knx_main" + std::to_string(controlList.back().GroupAddress.GetMainGroup()) + "_" +
                               "middle" + std::to_string(controlList.back().GroupAddress.GetMiddleGroup());
        std::string deviceTitle = std::string(mainGroupName) + " :: " + middleGroupName;
        Config.AddDevice(deviceId, deviceTitle, controlList);
    }
}

void TEtsConfigTool::ParseGroupAddress(tinyxml2::XMLElement* mainGroup)
{
    auto mainGroupName = mainGroup->Attribute("Name");

    std::vector<knx::tool::TControlConfig> controlList;
    for (auto groupAddress = mainGroup->FirstChildElement(); groupAddress != nullptr;
         groupAddress = groupAddress->NextSiblingElement())
    {
        AddToControlConfig(groupAddress, controlList);
    }

    if (!controlList.empty()) {
        std::string deviceId = "knx_main" + std::to_string(controlList.back().GroupAddress.GetMainGroup());
        std::string deviceTitle = std::string(mainGroupName);
        Config.AddDevice(deviceId, deviceTitle, controlList);
    }
}

void TEtsConfigTool::ValidateWbMqttConfig(const std::string& schemaPath)
{
    Config.Validate(schemaPath);
}

Json::Value TEtsConfigTool::GetWbMqttConfig()
{
    return Config.GetRoot();
}
