#include "datapointutils.h"
#include "dptjsonbuilder.h"

namespace knx
{
    namespace object
    {

        TDptJsonField::TDptJsonField(const std::string& name, TDptJsonField::EFieldType type, uint32_t width)
            : Name(name),
              FieldType(type),
              BitWidth(width)
        {}
        std::string TDptJsonField::GetName() const
        {
            return Name;
        }
        uint32_t TDptJsonField::GetBitWidth() const
        {
            return BitWidth;
        }
        void TDptJsonField::SetValue(std::bitset<64> value)
        {
            switch (FieldType) {
                case EFieldType::CHAR: {
                    std::string str(1, static_cast<char>(value.to_ullong()));
                    Value = Json::Value(str);
                } break;
                case EFieldType::BIT:
                    Value = Json::Value(value.test(0));
                    break;
                case EFieldType::UNSIGNED_INT:
                case EFieldType::ENUM:
                    Value = Json::Value(value.to_ullong() & ((1 << BitWidth) - 1));
                    break;
                case EFieldType::INT:
                    if (BitWidth == 8) {
                        Value = Json::Value(static_cast<int8_t>(value.to_ulong() & 0xFF));
                    } else if (BitWidth == 16) {
                        Value = Json::Value(static_cast<int16_t>(value.to_ulong() & 0xFFFF));
                    } else if (BitWidth == 32) {
                        Value = Json::Value(static_cast<int32_t>(value.to_ulong()));
                    } else if (BitWidth == 64) {
                        Value = Json::Value(static_cast<int64_t>(value.to_ullong()));
                    }
                    break;
                case EFieldType::FLOAT: {
                    if (BitWidth == 16) {
                        Value = Json::Value(datapointUtils::RawToFloat16(value.to_ulong()));
                    } else if (BitWidth == 32) {
                        Value = Json::Value(datapointUtils::RawToFloat32(value.to_ulong()));
                    }
                } break;
            }
        }
        void TDptJsonField::SetValue(const Json::Value& value)
        {
            Value = value;
        }
        std::bitset<64> TDptJsonField::GetRawValue() const
        {
            switch (FieldType) {
                case EFieldType::CHAR: {
                    auto str = Value.asString();
                    if (!str.empty()) {
                        return static_cast<uint8_t>(str[0]);
                    }
                    return 0;
                }
                case EFieldType::BIT:
                    return Value.asBool();
                case EFieldType::ENUM:
                case EFieldType::UNSIGNED_INT:
                    return Value.asUInt64();
                case EFieldType::INT:
                    return Value.asInt64();
                case EFieldType::FLOAT: {
                    if (BitWidth == 16) {
                        return datapointUtils::FloatToRaw16(Value.asFloat());
                    } else if (BitWidth == 32) {
                        return datapointUtils::FloatToRaw32(Value.asFloat());
                    }
                } break;
            }
            return {}; // TODO!!!
        }
        Json::Value TDptJsonField::GetValue() const
        {
            return Value;
        }

    }
}