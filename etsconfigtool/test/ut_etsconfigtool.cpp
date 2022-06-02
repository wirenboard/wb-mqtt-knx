#include "config.h"
#include "etsconfigtool.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

class EtsConfigToolTest: public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}

    std::string TestConfigDir = std::string(CMAKE_SOURCE_DIR) + "/etsconfigtool/test/config/";
    std::string SchemaPath = std::string(CMAKE_SOURCE_DIR) + "/wb-mqtt-knx.schema.json";
};

TEST_F(EtsConfigToolTest, SaveConfig)
{
    char tempConfigFilePath[] = "/tmp/etsconfigtool.XXXXXX";
    EXPECT_NE(mkstemp(tempConfigFilePath), -1);
    std::string tempConfigFilePathStr = tempConfigFilePath;

    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml");

    converter.SaveWbMqttConfig(tempConfigFilePathStr);

    auto configRoot = WBMQTT::JSON::Parse(tempConfigFilePathStr);
    unlink(tempConfigFilePathStr.c_str());

    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));
}

TEST_F(EtsConfigToolTest, LoadEtsExport3Level)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml");

    EXPECT_NO_THROW(converter.ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExport2Level)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_2level_style.xml");

    EXPECT_NO_THROW(converter.ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExportFreeLevel)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_free_level_style.xml");

    EXPECT_NO_THROW(converter.ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExportConfigNotExist)
{
    knx::tool::TEtsConfigTool converter;
    EXPECT_THROW(converter.LoadEtsExport(TestConfigDir + "ets_export_not_exist.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, LoadEtsExportRootCorrupt)
{
    knx::tool::TEtsConfigTool converter;
    EXPECT_THROW(converter.LoadEtsExport(TestConfigDir + "ets_export_root_corrupt.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, LoadEtsExportNoRoot)
{
    knx::tool::TEtsConfigTool converter;
    EXPECT_THROW(converter.LoadEtsExport(TestConfigDir + "ets_export_no_root.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, CheckConfig)
{
    std::vector<std::string> configNameList = {"ets_export_free_level_style.xml",
                                               "ets_export_2level_style.xml",
                                               "ets_export_3level_style.xml"};
    for (const auto& configName: configNameList) {
        knx::tool::TEtsConfigTool converter;
        EXPECT_NO_THROW(converter.LoadEtsExport(TestConfigDir + configName));

        Json::Value root = converter.GetWbMqttConfig();

        EXPECT_EQ(root["configVersion"].asInt(), 1);
        EXPECT_EQ(root["debug"].asBool(), false);
        EXPECT_TRUE(!root["devices"].empty());

        for (const auto& device: root["devices"]) {
            EXPECT_TRUE(!device["deviceId"].empty());
            EXPECT_TRUE(!device["deviceTitle"].empty());

            EXPECT_THAT(device["deviceId"].asString(), StartsWith("knx_main"));

            for (const auto& control: device["controls"]) {
                EXPECT_TRUE(!control["controlId"].empty());
                EXPECT_TRUE(!control["controlTitle"].empty());
                EXPECT_TRUE(!control["dataPointType"].empty());
                EXPECT_TRUE(!control["groupAddress"].empty());
                EXPECT_TRUE(!control["readOnly"].empty());

                std::unique_ptr<knx::TKnxGroupAddress> address;
                EXPECT_NO_THROW(address = std::make_unique<knx::TKnxGroupAddress>(control["groupAddress"].asString()));

                EXPECT_EQ(control["controlId"].asString(),
                          std::string("control") + std::to_string(address->GetMainGroup()) + "_" +
                              std::to_string(address->GetMiddleGroup()) + "_" + std::to_string(address->GetSubGroup()));
                EXPECT_FALSE(control["readOnly"].asBool());
            }
        }
    }
}

TEST_F(EtsConfigToolTest, CheckConfigWithoutDtp)
{
    knx::tool::TEtsConfigTool converter;
    EXPECT_NO_THROW(converter.LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml"));
    Json::Value root = converter.GetWbMqttConfig();
    EXPECT_EQ("Raw_Value", root["devices"][2]["controls"][0]["dataPointType"].asString());
}
