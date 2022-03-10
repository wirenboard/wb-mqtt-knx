#pragma once

#include "../knxexception.h"
#include "dptjson.h"
#include "idpt.h"
#include <bitset>
#include <regex>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        class TDptJsonBuilder
        {
        public:
            explicit TDptJsonBuilder(const std::string& descriptorPath, const std::string& descriptorSchemaPath);

            PDpt Create(uint32_t id, bool hasSubId = false, uint32_t subId = 0);

        private:
            Json::Value DescriptorRoot;
            Json::Value SchemaRoot;
            std::map<uint32_t, Json::Value> DescriptorMap;
        };

    }
}