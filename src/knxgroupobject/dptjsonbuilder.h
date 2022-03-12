#pragma once

#include "../knxexception.h"
#include "datapointid.h"
#include "dptjson.h"
#include "idpt.h"
#include "idptbuilder.h"
#include <bitset>
#include <regex>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        /// \brief Class for creating a JSON Datapoint based on the descriptor JSON file
        class TDptJsonBuilder: public IDptBuilder
        {
        public:
            /// Constructor
            /// \param descriptorPath Path to the JSON Datapoint Descriptor file
            /// \param descriptorSchemaPath Scheme for verification The JSON Datapoint Descriptor File
            explicit TDptJsonBuilder(const std::string& descriptorPath, const std::string& descriptorSchemaPath);

            PDpt Create(const TDatapointId& datapointId) override;

        private:
            Json::Value DescriptorRoot;
            Json::Value SchemaRoot;
            std::map<uint32_t, Json::Value> DescriptorMap;
        };

    }
}