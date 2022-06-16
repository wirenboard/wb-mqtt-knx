#pragma once

#include "../../src/knxgroupobject/dptjsonbuilder.h"
#include "../../src/knxgroupobject/dptwbmqttbuilder.h"
#include "config.h"
#include "mqttknxconfig.h"
#include "tinyxml2.h"
#include <string>

namespace knx
{
    namespace tool
    {
        /// \brief Class for reading ETS export and saving to a config file
        class TEtsConfigTool
        {
        public:
            /// Constructor
            /// \param mqttBuilder mqtt datapoint builder
            /// \param jsonBuilder json datapoint builder
            /// \param defaultId default datapoint ID if a datapoint description not found
            TEtsConfigTool(const object::IDptBuilder& mqttBuilder,
                           const object::IDptBuilder& jsonBuilder,
                           const object::TDatapointId& defaultId);

            /// Load ETS export
            /// \param filePath ETS export xml file path
            /// \exception knx::TKnxException If the export file is missing or has an incorrect format
            void LoadEtsExport(const std::string& filePath);

            /// Check configuration file according to scheme
            /// \param schemaPath path to the json file of the schema
            /// \exception std::runtime_error Throws on validation error
            void ValidateWbMqttConfig(const std::string& schemaPath);

            /// Get JSON Root Object from Config
            /// \return root object from config
            Json::Value GetWbMqttConfig();

            /// Save wb-mqtt-knx config
            /// \param filePath file path for saving
            void SaveWbMqttConfig(const std::string& filePath);

        private:
            void ParseGroupAddress(tinyxml2::XMLElement* mainGroup);
            void ParseGroupAddress(tinyxml2::XMLElement* mainGroup, tinyxml2::XMLElement* middleGroup);

            std::string DatapointTypeExportToConfig(const std::string& dpts);
            void AddToControlConfig(tinyxml2::XMLElement* groupAddress,
                                    std::vector<knx::tool::TControlConfig>& controlList);
            TMqttKnxConfig Config;
            const object::IDptBuilder& MqttBuilder;
            const object::IDptBuilder& JsonBuilder;
            const object::TDatapointId DefaultId;
        };
    }
}