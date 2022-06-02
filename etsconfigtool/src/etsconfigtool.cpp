#include "etsconfigtool.h"
#include "../src/knxexception.h"
#include "../src/knxgroupobject/datapointpool.h"
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

    inline std::string DatapointTypeExportToConfigDefault()
    {
        return knx::object::DataPointPool::GetDataPointNameById(0, 0);
    }

    std::string DatapointTypeExportToConfig(const std::string& dpts)
    {
        uint32_t generalType;
        uint32_t subType = 0;

        try {
            auto firstDpts = WBMQTT::StringSplit(dpts, ",").at(0);
            auto firstDptsTokens = WBMQTT::StringSplit(firstDpts, "-");
            generalType = std::stoi(firstDptsTokens.at(1));
            if (firstDptsTokens.size() == 3) {
                subType = std::stoi(firstDptsTokens.at(2));
            }

            return knx::object::DataPointPool::GetDataPointNameById(generalType, subType);
        } catch (const std::out_of_range& oor) {
            std::cerr << "DatapointTypeExportToConfig( " << dpts << " ): Out of Range error: " << oor.what() << '\n';
            return "";
        }
    }

    void AddToControlConfig(tinyxml2::XMLElement* groupAddress, std::vector<knx::tool::TControlConfig>& controlList)
    {
        auto name = groupAddress->Attribute("Name");
        auto address = groupAddress->Attribute("Address");
        auto dpts = groupAddress->Attribute("DPTs");
        if ((name != nullptr) && (address != nullptr)) {
            TControlConfig control;
            knx::TKnxGroupAddress knxGroupAddress(address);
            auto groupAddressStr = knxGroupAddress.ToString();
            std::replace(groupAddressStr.begin(), groupAddressStr.end(), '/', '_');
            control.Id = std::string("control") + groupAddressStr;
            control.Title = name;
            control.GroupAddress = knxGroupAddress;
            if (dpts != nullptr) {
                control.DatapointType = DatapointTypeExportToConfig(dpts);
            } else {
                control.DatapointType = DatapointTypeExportToConfigDefault();
            }
            control.ReadOnly = DEFAULT_IS_CONTROL_READONLY;
            if (!control.DatapointType.empty()) {
                controlList.push_back(control);
            }
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
