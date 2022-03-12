#pragma once

#include "idpt.h"
#include <bitset>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        /// \brief Class for setting up json datapoint fields
        class TDptJsonField
        {
        public:
            /// JSON Field type
            enum class EFieldType
            {
                CHAR,
                BIT,
                UNSIGNED_INT,
                INT,
                ENUM,
                FLOAT
            };

            /// Constructor
            /// \param name The name that will be displayed in the JSON object
            /// \param type field type
            /// \param width Field Width in KNX Telegram
            explicit TDptJsonField(const std::string& name, EFieldType type, uint32_t width);

            /// Getter field name
            /// \return field name
            std::string GetName() const;

            /// Getter field width
            /// \return field width
            uint32_t GetBitWidth() const;

            /// Set field value from raw value of a KNX telegram
            /// \param value raw value from a KNX telegramm
            void SetValue(std::bitset<64> value);

            /// Set field value
            /// \param value JSON value
            void SetValue(const Json::Value& value);

            /// Get raw value from a KNX telegram
            /// \return raw value
            std::bitset<64> GetRawValue() const;

            /// Get JSON value
            /// \return JSON value
            Json::Value GetValue() const;

        private:
            std::string Name;
            EFieldType FieldType;
            uint32_t BitWidth;
            Json::Value Value;
        };

    }
}