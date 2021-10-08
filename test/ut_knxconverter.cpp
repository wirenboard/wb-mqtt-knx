#include "../src/knxconverter.h"
#include "../src/knxexception.h"
#include <gtest/gtest.h>

TEST(KnxConverterTest, FromKnxDataSetCheck)
{
    const std::vector<std::pair<std::vector<uint8_t>, std::string>> knxDataSet = {
        {{0xbc, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28},
         "i:0/0/1 i:9/7/55 GroupValueWrite 0xcf 0x0e 0xff "},
        {{0xbc, 0x00, 0x00, 0x4f, 0x37, 0xf1, 0x00, 0x00, 0xca}, "i:0/0/0 g:9/7/55 GroupValueRead 0x00"},
        {{0xbc, 0x11, 0x02, 0x09, 0x64, 0xe1, 0x00, 0x81, 0x5d}, "i:1/1/2 g:1/1/100 GroupValueWrite 0x01"}};

    for (const auto& knxData : knxDataSet) {
        knx::TTelegram telegram(knxData.first);
        auto           mqttString = knx::TKnxConverter::KnxTelegramToMqtt(telegram);

        EXPECT_EQ(mqttString, knxData.second);
    }
}

TEST(KnxConverterTest, FromMqttDataSetCheck)
{
    const std::vector<std::tuple<std::string, std::vector<uint8_t>, bool, eibaddr_t, eibaddr_t>> mqttDataSet =
        {{"i:0/0/1:9/7/55 GroupValueWrite 0x0 0xcf 14 0xff",
          {0x00, 0x80, 0xcf, 0x0e, 0xff},
          false,
          0x1,
          0x9737},
         {"g:9/7/55 GroupValueRead", {0x00, 0x00}, true, 0x00, 0x4f37},
         {"g:9/7/50 GroupValueWrite", {0x00, 0x80}, true, 0x00, 0x4f32}};

    for (const auto& mqttData : mqttDataSet) {
        auto telegram = knx::TKnxConverter::MqttToKnxTelegram(std::get<0>(mqttData));

        EXPECT_EQ(telegram->GetTPDUPayload(), std::get<1>(mqttData));
        EXPECT_EQ(telegram->IsGroupAddressed(), std::get<2>(mqttData));
        EXPECT_EQ(telegram->GetSourceAddress(), std::get<3>(mqttData));
        EXPECT_EQ(telegram->GetReceiverAddress(), std::get<4>(mqttData));
    }
}

TEST(KnxConverterTest, FromMqttDataSetFail)
{
    const std::vector<std::string> mqttDataSet = {"i:0/0/1:9/7/55 GroupValueAAWrite 0x0 0xcf 14 0xff",
                                                  "g: GroupValueRead",
                                                  "g:9/7/50 GroupValueWrite er eer ewr"};

    for (const auto& mqttData : mqttDataSet) {
        EXPECT_THROW(knx::TKnxConverter::MqttToKnxTelegram(mqttData), knx::TKnxException)
            << "No throw set: " << mqttData;
    }
}