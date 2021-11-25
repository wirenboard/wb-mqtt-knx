#include "etsconfigtool.h"
#include "../src/knxexception.h"
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

    std::string DatapointTypeExportToConfig(const std::string& dpts)
    {
        auto token = WBMQTT::StringSplit(dpts, ",");

        uint32_t generalType = std::stoi(WBMQTT::StringSplit(WBMQTT::StringSplit(dpts, ",").at(0), "-").at(1));

        static const std::unordered_map<uint32_t, std::string> generalTypeMap = {{1, "1.xxx_B1"},
                                                                                 {2, "2.xxx_B2"},
                                                                                 {5, "5.xxx_8-Bit_Unsigned_Value"},
                                                                                 {6, "6.xxx_V8"},
                                                                                 {9, "9.xxx_2-Octet_Float_Value"}};
        auto it = generalTypeMap.find(generalType);
        if (it != generalTypeMap.end()) {
            return it->second;
        }
        return "Raw_Value";
    }

    void AddToControlConfig(tinyxml2::XMLElement* groupAddress, std::vector<knx::tool::TControlConfig>& controlList)
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
        if (result == tinyxml2::XMLError::XML_ERROR_FILE_NOT_FOUND) {
            wb_throw(TKnxException, "ETS export file not found");
        } else {
            wb_throw(TKnxException,
                     "tinyxml2::LoadFile() error= XMLError::" + std::to_string(static_cast<int32_t>(result)));
        }
    }
    auto root = xmlDoc.RootElement();

    if (root == nullptr) {
        wb_throw(TKnxException, "ETS export RootElement is corrupt");
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
