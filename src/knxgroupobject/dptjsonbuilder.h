#pragma once

#include "dptjson.h"
#include "idpt.h"
#include <bitset>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        class TDptJsonBuilder
        {
        public:
            explicit TDptJsonBuilder(const std::string& descriptorPath, const std::string& descriptorSchemaPath)
            {
                DescriptorRoot = WBMQTT::JSON::Parse(descriptorPath);
                SchemaRoot = WBMQTT::JSON::Parse(descriptorSchemaPath);

                WBMQTT::JSON::Validate(DescriptorRoot, SchemaRoot);
            }

            PDpt Create(uint32_t id, uint32_t subId)
            {
                return nullptr;
            }

        private:
            Json::Value DescriptorRoot;
            Json::Value SchemaRoot;
        };

    }
}