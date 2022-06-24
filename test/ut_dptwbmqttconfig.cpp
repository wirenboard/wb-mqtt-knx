#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptwbmqttconfig.h"
#include "testutils.h"
#include "gtest/gtest.h"

class DptWbMqttConfigTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        DptWbMqttConfig = std::make_unique<knx::object::TDptWbMqttConfig>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptWbMqttConfig> DptWbMqttConfig;
};

TEST_F(DptWbMqttConfigTest, getConNameTest)
{
    auto name = DptWbMqttConfig->GetDptConfigName(knx::object::TDatapointId{5});
    ASSERT_TRUE(name);
    EXPECT_EQ("5.xxx_8-Bit_Unsigned_Value", *name);
}

TEST_F(DptWbMqttConfigTest, getConfigNameNegTest)
{
    auto name = DptWbMqttConfig->GetDptConfigName(knx::object::TDatapointId{5000});
    EXPECT_FALSE(name);
}

TEST_F(DptWbMqttConfigTest, getDefaultConfigNameTest)
{
    auto name = DptWbMqttConfig->GetDptConfigName(knx::object::TDptWbMqttConfig::DefaultDatapointId);
    ASSERT_TRUE(name);
    EXPECT_EQ("Raw_Value", *name);
}
