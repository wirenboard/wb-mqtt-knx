#include "config.h"
#include "etsconfigtool.h"
#include "gtest/gtest.h"

class EtsConfigToolTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        char tempConfigFilePath[] = "/tmp/etsconfigtool.XXXXXX";
        EXPECT_NE(mkstemp(tempConfigFilePath), -1);
        TempConfigFilePath = tempConfigFilePath;
    }

    void TearDown() override
    {
        unlink(TempConfigFilePath.c_str());
    }

    std::string TestConfigDir = std::string(CMAKE_SOURCE_DIR) + "/etsconfigtool/test/config/";
    std::string SchemaPath = std::string(CMAKE_SOURCE_DIR) + "/wb-mqtt-knx.schema.json";
    std::string TempConfigFilePath;
};

TEST_F(EtsConfigToolTest, LoadEtsExport3Level)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_3level_style.xml");

    converter.SaveWbMqttConfig(TempConfigFilePath);

    auto configRoot = WBMQTT::JSON::Parse(TempConfigFilePath);
    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));
}

TEST_F(EtsConfigToolTest, LoadEtsExport2Level)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_2level_style.xml");

    converter.SaveWbMqttConfig(TempConfigFilePath);

    auto configRoot = WBMQTT::JSON::Parse(TempConfigFilePath);
    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));
}

TEST_F(EtsConfigToolTest, LoadEtsExportFreeLevel)
{
    knx::tool::TEtsConfigTool converter;
    converter.LoadEtsExport(TestConfigDir + "ets_export_free_level_style.xml");

    converter.SaveWbMqttConfig(TempConfigFilePath);

    auto configRoot = WBMQTT::JSON::Parse(TempConfigFilePath);
    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));
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