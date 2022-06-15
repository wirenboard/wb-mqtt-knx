#include "dptjsonbuilder.h"

/// Json datapoint description file documentation:
/// https://github.com/wirenboard/wb-mqtt-knx/blob/master/jsondatapoint.md

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
            DatapointNameMapUpdate();
        }

        PDpt TDptJsonBuilder::Create(const TDatapointId& datapointId)
        {
            if (!HasDpt(datapointId)) {
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

        bool TDptJsonBuilder::HasDpt(const TDatapointId& datapointId)
        {
            try {
                GetDptName(datapointId);
            } catch (const TKnxException&) {
                return false;
            }
            return true;
        }

        void TDptJsonBuilder::DatapointNameMapUpdate()
        {
            for (const auto& el: DescriptorMap) {
                auto mainId = el.first;
                auto& descriptor = el.second;
                const TDatapointId id{mainId};
                DatapointNameMap[id] = id.ToString() + "_" + descriptor["name"].asString() + "_JSON";

                for (const auto& datapoint: descriptor["datapoint"]) {
                    auto datapointId = id;
                    datapointId.SetSub(datapoint["subid"].asUInt());
                    DatapointNameMap[datapointId] =
                        datapointId.ToString() + "_" + datapoint["name"].asString() + "_JSON";
                }
            }
        }

        std::string TDptJsonBuilder::GetDptName(const TDatapointId& datapointId) const
        {
            auto it = DatapointNameMap.find(datapointId);
            if (it != DatapointNameMap.end()) {
                return it->second;
            }
            auto id = datapointId;
            id.ClearSub();
            it = DatapointNameMap.find(datapointId);
            if (it != DatapointNameMap.end()) {
                return it->second;
            }

            wb_throw(TKnxException, "Can not found Datapoint Name from Id");
        }

    }
}