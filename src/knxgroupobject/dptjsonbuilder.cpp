#include "dptjsonbuilder.h"

namespace
{
    struct EncodingResult
    {
        char typeCode;
        uint32_t width;
    };

    std::map<char, knx::object::TDptJsonField::EFieldType> EncodingMap = {
        {'B', knx::object::TDptJsonField::EFieldType::BIT},
        {'N', knx::object::TDptJsonField::EFieldType::ENUM},
        {'A', knx::object::TDptJsonField::EFieldType::CHAR},
        {'U', knx::object::TDptJsonField::EFieldType::UNSIGNED_INT},
        {'V', knx::object::TDptJsonField::EFieldType::INT},
        {'F', knx::object::TDptJsonField::EFieldType::FLOAT}};

    std::vector<EncodingResult> ParseEncodingString(const std::string& str)
    {
        auto encodingStr = str;
        std::regex encodingRegex("([BNAUVFr])([0-9]+)");
        std::smatch sm;
        std::vector<EncodingResult> encodedFieldList;

        while (std::regex_search(encodingStr, sm, encodingRegex)) {
            auto fieldType = sm.str(1)[0];
            auto fieldSize = std::stoul(sm.str(2));
            if (fieldType == 'B') {
                for (uint32_t i = 0; i < fieldSize; ++i) {
                    encodedFieldList.push_back({fieldType, 1});
                }
            } else {
                encodedFieldList.push_back({fieldType, fieldSize});
            }
            encodingStr = sm.suffix();
        }
        return encodedFieldList;
    }
}

namespace knx
{
    namespace object
    {
        TDptJsonBuilder::TDptJsonBuilder(const std::string& descriptorPath)
        {
            DescriptorRoot = WBMQTT::JSON::Parse(descriptorPath);

            for (const auto& descriptor: DescriptorRoot) {
                if (!DescriptorMap.insert({descriptor["id"].asUInt(), descriptor}).second) {
                    wb_throw(TKnxException, "Duplicate ID in JSON dpt descriptor");
                }
            }
        }

        PDpt TDptJsonBuilder::Create(const TDatapointId& datapointId)
        {
            if (DescriptorMap.find(datapointId.GetMain()) == DescriptorMap.end()) {
                return nullptr;
            }
            auto descriptor = DescriptorMap[datapointId.GetMain()];

            auto fieldJsonValue = descriptor["field"];
            if (datapointId.HasSubId()) {
                auto subId = datapointId.GetSub();
                auto datapointJsonArray = descriptor["datapoint"];
                auto datapointIterator =
                    std::find_if(datapointJsonArray.begin(),
                                 datapointJsonArray.end(),
                                 [subId](const Json::Value& value) { return value["subid"].asUInt() == subId; });
                if (datapointIterator != datapointJsonArray.end()) {
                    fieldJsonValue = (*datapointIterator)["field"];
                }
            }
            auto encodingStr = descriptor["encoding"].asString();
            auto encodedFieldList = ParseEncodingString(encodingStr);
            uint32_t sumSize = 0;
            for (const auto& field: encodedFieldList) {
                sumSize += field.width;
            }
            auto dptJson = std::make_shared<TDptJson>();
            auto jsonFieldIterator = fieldJsonValue.begin();
            uint32_t bitPosition = sumSize <= 6 ? 8U - sumSize : 8U;
            for (const auto& encodedField: encodedFieldList) {
                if (encodedField.typeCode != 'r') {
                    TDptJsonField field((*jsonFieldIterator)["name"].asString(),
                                        EncodingMap[encodedField.typeCode],
                                        encodedField.width);
                    dptJson->AddField(field, bitPosition);
                    ++jsonFieldIterator;
                }
                bitPosition += encodedField.width;
            }
            return dptJson;
        }

    }
}