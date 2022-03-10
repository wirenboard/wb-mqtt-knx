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
}

namespace knx
{
    namespace object
    {
        TDptJsonBuilder::TDptJsonBuilder(const std::string& descriptorPath, const std::string& descriptorSchemaPath)
        {
            DescriptorRoot = WBMQTT::JSON::Parse(descriptorPath);
            if (!descriptorSchemaPath.empty()) {
                SchemaRoot = WBMQTT::JSON::Parse(descriptorSchemaPath);
                WBMQTT::JSON::Validate(DescriptorRoot, SchemaRoot);
            }
            for (const auto& descriptor: DescriptorRoot) {
                if (!DescriptorMap.insert({descriptor["id"].asUInt(), descriptor}).second) {
                    wb_throw(TKnxException, "Duplicate ID in JSON dpt descriptor"); // TODO Add description
                }
            }
        }

        PDpt TDptJsonBuilder::Create(uint32_t id, bool hasSubId, uint32_t subId)
        {
            if (DescriptorMap.find(id) == DescriptorMap.end()) {
                return nullptr;
            }
            auto descriptor = DescriptorMap[id];
            auto encodingStr = descriptor["encoding"].asString();
            std::regex encodingRegex("([a-zA-Z])([0-9]+)");
            std::smatch sm;
            std::vector<EncodingResult> encodedFieldList;
            uint32_t sumSize = 0;
            while (std::regex_search(encodingStr, sm, encodingRegex)) {
                auto fieldType = sm.str(1)[0];
                auto fieldSize = std::stoul(sm.str(2));
                sumSize += fieldSize;

                encodedFieldList.push_back({fieldType, fieldSize});
                encodingStr = sm.suffix();
            }
            auto dptJson = std::make_shared<TDptJson>();
            auto jsonFieldIterator = descriptor["field"].begin();
            uint32_t bitPosition = sumSize <= 6 ? 8U - sumSize : 8U;
            for (const auto& encodedField: encodedFieldList) {
                if (encodedField.typeCode != 'r') {
                    if (encodedField.typeCode == 'B') {
                        for (uint32_t i = 0; i < encodedField.width; ++i) {
                            TDptJsonField field((*jsonFieldIterator)["name"].asString(),
                                                EncodingMap[encodedField.typeCode],
                                                1);
                            dptJson->AddField(field, bitPosition);
                            ++jsonFieldIterator;
                            ++bitPosition;
                        }
                    } else {
                        TDptJsonField field((*jsonFieldIterator)["name"].asString(),
                                            EncodingMap[encodedField.typeCode],
                                            encodedField.width);
                        dptJson->AddField(field, bitPosition);
                        ++jsonFieldIterator;
                        bitPosition += encodedField.width;
                    }
                } else {
                    bitPosition += encodedField.width;
                }
            }
            return dptJson;
        }

    }
}