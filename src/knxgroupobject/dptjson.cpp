#include "dptjson.h"
#include "../knxexception.h"
#include "datapointerror.h"
#include <algorithm>

namespace knx
{
    namespace object
    {
        void TDptJson::AddField(const TDptJsonField& field, uint32_t BitBeginPosition)
        {
            JsonFieldList.push_back({field, BitBeginPosition});
            uint32_t maxByte = (BitBeginPosition + field.GetBitWidth() - 1) / 8 + 1;
            PayloadBytesNum = std::max(PayloadBytesNum, maxByte);
        }

        std::vector<DptDescriptorField> TDptJson::getDescriptor() const
        {
            return {{"JSON", "text", 0, 255}};
        }

        void TDptJson::FromMqtt(uint32_t controlIndex, const WBMQTT::TAny& value)
        {
            if (controlIndex == 0) {
                std::stringstream sstr(value.As<std::string>());
                Json::Value jsonObject;
                sstr >> jsonObject;
                for (const auto& memberName: jsonObject.getMemberNames()) {
                    auto fieldIterator = std::find_if(JsonFieldList.begin(),
                                                      JsonFieldList.end(),
                                                      [memberName](const FieldWithPosition& field) {
                                                          return field.JsonField.GetName() == memberName;
                                                      });
                    if (fieldIterator != JsonFieldList.end()) {
                        fieldIterator->JsonField.SetValue(jsonObject[memberName]);
                    } else {
                        wb_throw(TKnxException, "Incorrect JSON object"); // TODO Add description
                    }
                }
            } else {
                wb_throw(TKnxException, datapointError::MQTT_INVALID_INDEX);
            }
        }

        void TDptJson::FromKnx(const std::vector<uint8_t>& payload)
        {
            if (payload.size() != PayloadBytesNum) {
                wb_throw(TKnxException, "Incorrect payload size"); // TODO Add description
            }
            for (auto& field: JsonFieldList) {
                TJsonFieldRawValue rawValue;
                uint32_t bitCount = field.BitBeginPosition + field.JsonField.GetBitWidth() - 1;
                uint32_t bitPosition = 0;
                for (uint32_t i = bitCount; i >= field.BitBeginPosition; --i) {
                    if (payload[i / 8] & (1 << (7 - i % 8))) {
                        rawValue.set(bitPosition);
                    }
                    ++bitPosition;
                }
                field.JsonField.SetValue(rawValue);
            }
        }

        std::vector<uint8_t> TDptJson::ToKnx()
        {
            std::vector<uint8_t> payload(PayloadBytesNum);
            for (auto& field: JsonFieldList) {
                uint32_t rawValuePosition = 0;
                for (uint32_t i = (field.BitBeginPosition + field.JsonField.GetBitWidth() - 1);
                     i >= field.BitBeginPosition;
                     --i) {
                    if (field.JsonField.GetRawValue().test(rawValuePosition)) {
                        payload[i / 8] |= (1 << (7 - i % 8));
                    }
                    ++rawValuePosition;
                }
            }
            return payload;
        }

        std::vector<WBMQTT::TAny> TDptJson::ToMqtt()
        {
            Json::Value jsonObject;
            for (const auto& field: JsonFieldList) {
                jsonObject[field.JsonField.GetName()] = field.JsonField.GetValue();
            }
            Json::StreamWriterBuilder builder;
            builder["indentation"] = ""; // If you want whitespace-less output
            return {Json::writeString(builder, jsonObject)};
        }
    }
}