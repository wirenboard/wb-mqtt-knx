#include "../../src/knxgroupobject/dptjsonconfig.h"
#include "../../src/knxgroupobject/dptwbmqttconfig.h"
#include "config.h"
#include "etsconfigtool.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

class EtsConfigToolTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Converter = std::make_unique<knx::tool::TEtsConfigTool>(DptWbMqttConfig,
                                                                DptJsonConfig,
                                                                knx::object::TDptWbMqttConfig::DefaultDatapointId);
    }

    void TearDown() override
    {}

    std::string TestConfigDir = std::string(CMAKE_SOURCE_DIR) + "/etsconfigtool/test/config/";
    std::string SchemaPath = std::string(CMAKE_SOURCE_DIR) + "/wb-mqtt-knx.schema.json";
    std::unique_ptr<knx::tool::TEtsConfigTool> Converter;
    knx::object::TDptJsonConfig DptJsonConfig{std::string(CMAKE_SOURCE_DIR) + "/wb-mqtt-knx-jsondpt.conf"};
    knx::object::TDptWbMqttConfig DptWbMqttConfig;
};

TEST_F(EtsConfigToolTest, SaveConfig)
{
    char tempConfigFilePath[] = "/tmp/etsconfigtool.XXXXXX";
    EXPECT_NE(mkstemp(tempConfigFilePath), -1);
    std::string tempConfigFilePathStr = tempConfigFilePath;

    Converter->LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml");

    Converter->SaveWbMqttConfig(tempConfigFilePathStr);

    auto configRoot = WBMQTT::JSON::Parse(tempConfigFilePathStr);
    unlink(tempConfigFilePathStr.c_str());

    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));
}

TEST_F(EtsConfigToolTest, LoadEtsExport3Level)
{
    Converter->LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml");

    EXPECT_NO_THROW(Converter->ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExport2Level)
{
    Converter->LoadEtsExport(TestConfigDir + "ets_export_2level_style.xml");

    EXPECT_NO_THROW(Converter->ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExportFreeLevel)
{
    Converter->LoadEtsExport(TestConfigDir + "ets_export_free_level_style.xml");

    EXPECT_NO_THROW(Converter->ValidateWbMqttConfig(SchemaPath));
}

TEST_F(EtsConfigToolTest, LoadEtsExportConfigNotExist)
{
    EXPECT_THROW(Converter->LoadEtsExport(TestConfigDir + "ets_export_not_exist.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, LoadEtsExportRootCorrupt)
{
    EXPECT_THROW(Converter->LoadEtsExport(TestConfigDir + "ets_export_root_corrupt.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, LoadEtsExportNoRoot)
{
    EXPECT_THROW(Converter->LoadEtsExport(TestConfigDir + "ets_export_no_root.xml"), knx::TKnxException);
}

TEST_F(EtsConfigToolTest, CheckConfig)
{
    std::vector<std::string> configNameList = {"ets_export_free_level_style.xml",
                                               "ets_export_2level_style.xml",
                                               "ets_export_3level_style.xml"};
    for (const auto& configName: configNameList) {
        EXPECT_NO_THROW(Converter->LoadEtsExport(TestConfigDir + configName));

        Json::Value root = Converter->GetWbMqttConfig();

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

TEST_F(EtsConfigToolTest, MqttJsonConfig)
{
    EXPECT_NO_THROW(Converter->LoadEtsExport(TestConfigDir + "ets_export_wbmqtt_json_default.xml"));
    Json::Value root = Converter->GetWbMqttConfig();

    uint32_t i = 0;
    for (const auto& el: {"1.xxx_B1", "21.001_StatusGen_JSON", "Raw_Value"}) {
        EXPECT_EQ(el, root["devices"][0]["controls"][i]["dataPointType"].asString());
        ++i;
    }
}
