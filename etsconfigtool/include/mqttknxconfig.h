#pragma once
#include <../src/knxgroupaddress.h>
#include <string>
#include <vector>
#include <wblib/json_utils.h>

namespace knx
{
    namespace tool
    {
        /// Control configuration for a group object
        struct TControlConfig
        {
            std::string Id;                ///< Control prefix id
            std::string Title;             ///< Control title
            TKnxGroupAddress GroupAddress; ///< Group Address
            std::string DatapointType;     ///< Datapoint type
            bool ReadOnly;                 ///< Read Only flag
        };

        /// Class for work with wb-mqtt-knx config
        class TMqttKnxConfig
        {
        public:
            /// Add header to config
            /// \param configVersion Config version number
            /// \param debug debug flag
            /// \param enableLegacyKnxDevice enable legacy KNX device flag
            void AddHeader(uint32_t configVersion, bool debug, bool enableLegacyKnxDevice);

            /// Add MQTT Device to config
            /// \param id Device ID
            /// \param title Device Title
            /// \param controls controls list for this Device
            void AddDevice(const std::string& id,
                           const std::string& title,
                           const std::vector<TControlConfig>& controls);

            /// Save config file
            /// \param path the path to the Saved file
            void SaveConfig(const std::string& path);

        private:
            Json::Value RootObject;
        };
    }
}
