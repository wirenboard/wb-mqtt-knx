#include "config.h"
#include "etsconfigtool.h"
#include "gtest/gtest.h"

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
    auto schemaRoot = WBMQTT::JSON::Parse(SchemaPath);

    EXPECT_NO_THROW(WBMQTT::JSON::Validate(configRoot, schemaRoot));

    unlink(tempConfigFilePathStr.c_str());
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