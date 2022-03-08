#pragma once

#include "idpt.h"
#include <bitset>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {

        class TDptJsonField
        {
        public:
            enum class EFieldType
            {
                CHAR,
                BIT,
                UNSIGNED_INT,
                INT,
                ENUMERATE
            };

            explicit TDptJsonField(const std::string& name, EFieldType type, uint32_t width);

            std::string GetName() const;
            uint32_t GetBitWidth() const;

            void SetValue(std::bitset<64> value);
            void SetValue(const Json::Value& value);

            std::bitset<64> GetRawValue() const;

            Json::Value GetValue() const;

        private:
            std::string Name;
            EFieldType FieldType;
            uint32_t BitWidth;
            Json::Value Value;
        };

    }
}