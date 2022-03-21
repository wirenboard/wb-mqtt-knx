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
                wb_throw(TKnxException,
                         "Can't create JSON datapoint id: " + datapointId.ToString() +
                             ". There is no matching descriptor in the descriptor file.");
            }
            auto descriptor = DescriptorMap[datapointId.GetMain()];

            auto fieldJsonValue = descriptor["field"];
            auto encodingStr = descriptor["encoding"].asString();

            if (datapointId.HasSubId()) {
                auto subId = datapointId.GetSub();
                auto datapointJsonArray = descriptor["datapoint"];
                auto datapointIterator =
                    std::find_if(datapointJsonArray.begin(),
                                 datapointJsonArray.end(),
                                 [subId](const Json::Value& value) { return value["subid"].asUInt() == subId; });
                if (datapointIterator != datapointJsonArray.end()) {
                    if ((*datapointIterator).isMember("field")) {
                        fieldJsonValue = (*datapointIterator)["field"];
                    }
                    if ((*datapointIterator).isMember("encoding")) {
                        encodingStr = (*datapointIterator)["encoding"].asString();
                    }
                }
            }

            auto encodedFieldList = ParseEncodingString(encodingStr);
            uint32_t sumSize = 0;
            for (const auto& field: encodedFieldList) {
                sumSize += field.width;
            }
            auto dptJson = std::make_shared<TDptJson>();
            auto jsonFieldIterator = fieldJsonValue.begin();

            // This defines whether the first byte of the KNX payload can be filled.
            // If the fields are longer than 6 bits, the first byte of the payload is left blank.
            // In the case of a short payload, the number of blank bits before it is calculated.
            const auto shortPayload = (sumSize <= 6);
            uint32_t bitPosition = shortPayload ? 8U - sumSize : 8U;

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