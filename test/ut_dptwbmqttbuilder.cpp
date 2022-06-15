#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptwbmqttbuilder.h"
#include "config.h"
#include "testutils.h"
#include "gtest/gtest.h"

class DptWbMqttBuilderTest: public ::testing::Test
{
protected:
    std::string SourceDir = CMAKE_SOURCE_DIR;
    void SetUp() override
    {
        DptWbMqttBuilder = std::make_unique<knx::object::TDptWbMqttBuilder>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptWbMqttBuilder> DptWbMqttBuilder;
};

TEST_F(DptWbMqttBuilderTest, createDefaultDptTest)
{
    auto defaultDpt = DptWbMqttBuilder->Create(knx::object::TDptWbMqttBuilder::DefaultDatapointId);
    EXPECT_TRUE(defaultDpt);
    auto defaultDescriptor = (*defaultDpt)->getDescriptor();
    EXPECT_EQ(1, defaultDescriptor.size());
    EXPECT_EQ("RawValue", defaultDescriptor.front().Id);
}

TEST_F(DptWbMqttBuilderTest, createB1DptTest)
{
    auto dptB1 = DptWbMqttBuilder->Create(knx::object::TDatapointId{1, 1225});
    EXPECT_TRUE(dptB1);
    auto descriptorB1 = (*dptB1)->getDescriptor();
    EXPECT_EQ(1, descriptorB1.size());
    EXPECT_EQ("b", descriptorB1.front().Id);
}

TEST_F(DptWbMqttBuilderTest, createDptNegTest)
{
    auto dpt = DptWbMqttBuilder->Create(knx::object::TDatapointId{1000});
    EXPECT_FALSE(dpt);
}

TEST_F(DptWbMqttBuilderTest, getConfigNameTest)
{
    auto name = DptWbMqttBuilder->GetDptConfigName(knx::object::TDatapointId{5});
    EXPECT_TRUE(name);
    EXPECT_EQ("5.xxx_8-Bit_Unsigned_Value", *name);
}

TEST_F(DptWbMqttBuilderTest, getConfigNameNegTest)
{
    auto name = DptWbMqttBuilder->GetDptConfigName(knx::object::TDatapointId{5000});
    EXPECT_FALSE(name);
}

TEST_F(DptWbMqttBuilderTest, getDefaultConfigNameTest)
{
    auto name = DptWbMqttBuilder->GetDptConfigName(knx::object::TDptWbMqttBuilder::DefaultDatapointId);
    EXPECT_TRUE(name);
    EXPECT_EQ("Raw_Value", *name);
}