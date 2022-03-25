#include "dptjsonfield.h"
#include "datapointutils.h"
#include "dptjsonbuilder.h"
#include <set>

namespace knx
{
    namespace object
    {
        namespace
        {
            constexpr auto KNX_CHAR_BIT_WIDTH = 8;
            constexpr auto KNX_CHAR_STRING_BIT_WIDTH = 112;

            const std::map<TDptJsonField::EFieldType, std::set<uint32_t>> SupportedFixedBitWidthList = {
                {TDptJsonField::EFieldType::CHAR, {KNX_CHAR_BIT_WIDTH, KNX_CHAR_STRING_BIT_WIDTH}},
                {TDptJsonField::EFieldType::INT, {8, 16, 32, 64}},
                {TDptJsonField::EFieldType::FLOAT, {16, 32}}};
        }

        TDptJsonField::TDptJsonField(const std::string& name, TDptJsonField::EFieldType type, uint32_t width)
            : Name(name),
              FieldType(type),
              BitWidth(width)
        {
            auto it = SupportedFixedBitWidthList.find(type);
            if (it != SupportedFixedBitWidthList.end()) {
                if (it->second.find(BitWidth) == it->second.end()) {
                    wb_throw(TKnxException, "Unsupported bit width for field: " + name);
                }
            }
        }

        std::string TDptJsonField::GetName() const
        {
            return Name;
        }
        uint32_t TDptJsonField::GetBitWidth() const
        {
            return BitWidth;
        }
        void TDptJsonField::SetValue(TJsonFieldRawValue value)
        {
            switch (FieldType) {
                case EFieldType::CHAR: {
                    if (BitWidth == KNX_CHAR_BIT_WIDTH) {
                        std::string str(1, static_cast<char>(value.to_ullong()));
                        Value = Json::Value(str);
                    } else if (BitWidth == KNX_CHAR_STRING_BIT_WIDTH) {
                        std::string str;
                        for (int i = 0; i < KNX_CHAR_STRING_BIT_WIDTH / 8; ++i) {
                            auto lowByte = (value & TJsonFieldRawValue(0xFF)).to_ulong();
                            if (lowByte != 0) {
                                str.insert(0, std::string(1, static_cast<char>(lowByte)));
                            }
                            value >>= 8;
                        }
                        Value = Json::Value(str);
                    }
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
        TJsonFieldRawValue TDptJsonField::GetRawValue() const
        {
            switch (FieldType) {
                case EFieldType::CHAR: {
                    if (BitWidth == KNX_CHAR_BIT_WIDTH) {
                        auto str = Value.asString();
                        if (!str.empty()) {
                            return static_cast<uint8_t>(str[0]);
                        } else {
                            return 0;
                        }
                    } else if (BitWidth == KNX_CHAR_STRING_BIT_WIDTH) {
                        TJsonFieldRawValue value;
                        auto str = Value.asString();
                        if (str.empty()) {
                            return value;
                        } else if (str.size() > (KNX_CHAR_STRING_BIT_WIDTH / 8)) {
                            str = str.substr(0, (KNX_CHAR_STRING_BIT_WIDTH / 8));
                        }

                        value = TJsonFieldRawValue(str[0]);
                        for (uint32_t i = 1; i < str.size(); ++i) {
                            value <<= 8;
                            value |= TJsonFieldRawValue(str[i]);
                        }
                        value <<= KNX_CHAR_STRING_BIT_WIDTH - (str.size() * 8);
                        return value;
                    }
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
                }
            }
            wb_throw(TKnxException, "Unsupported JSON field type or width");
        }
        Json::Value TDptJsonField::GetValue() const
        {
            return Value;
        }
    }
}