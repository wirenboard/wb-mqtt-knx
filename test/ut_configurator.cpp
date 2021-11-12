#include "../src/configurator.h"
#include "config.h"
#include "mock/knxgroupobjectcontroller_mock.h"
#include "mock/mqttdeviceadapter_mock.h"
#include <gtest/gtest.h>

class ConfiguratorTest: public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}

    std::string SourceDir = CMAKE_SOURCE_DIR;
};

TEST_F(ConfiguratorTest, GetConfig)
{
    EXPECT_NO_THROW(std::make_unique<knx::TConfigurator>(SourceDir + "/test/config/wb-mqtt-knx.conf",
                                                         SourceDir + "/test/config/wb-mqtt-knx.schema.json",
                                                         nullptr));
}

TEST_F(ConfiguratorTest, GetIncorrectConfig)
{
    EXPECT_THROW(std::make_unique<knx::TConfigurator>(SourceDir + "/test/config/wb-mqtt-knx_bad.conf",
                                                      SourceDir + "/test/config/wb-mqtt-knx.schema.json",
                                                      nullptr),
                 std::exception);
}

TEST_F(ConfiguratorTest, NoConfig)
{
    EXPECT_THROW(std::make_unique<knx::TConfigurator>(".", SourceDir + "/test/config/wb-mqtt-knx.schema.json", nullptr),
                 std::exception);
}