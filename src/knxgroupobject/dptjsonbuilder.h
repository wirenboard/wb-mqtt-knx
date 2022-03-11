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
        class TDptJsonBuilder: public IDptBuilder
        {
        public:
            explicit TDptJsonBuilder(const std::string& descriptorPath, const std::string& descriptorSchemaPath);

            PDpt Create(const TDatapointId& datapointId) override;

        private:
            Json::Value DescriptorRoot;
            Json::Value SchemaRoot;
            std::map<uint32_t, Json::Value> DescriptorMap;
        };

    }
}