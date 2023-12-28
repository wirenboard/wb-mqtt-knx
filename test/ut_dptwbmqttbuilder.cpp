#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptwbmqttbuilder.h"
#include "testutils.h"
#include <gtest/gtest.h>

class DptWbMqttBuilderTest: public ::testing::Test
{
protected:
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
    ASSERT_NE(defaultDpt, nullptr);
    auto defaultDescriptor = defaultDpt->getDescriptor();
    EXPECT_EQ(1, defaultDescriptor.size());
    EXPECT_EQ("RawValue", defaultDescriptor.front().Id);
}

TEST_F(DptWbMqttBuilderTest, createB1DptTest)
{
    auto dptB1 = DptWbMqttBuilder->Create(knx::object::TDatapointId{1, 1225});
    ASSERT_NE(dptB1, nullptr);
    auto descriptorB1 = dptB1->getDescriptor();
    EXPECT_EQ(1, descriptorB1.size());
    EXPECT_EQ("b", descriptorB1.front().Id);
}

TEST_F(DptWbMqttBuilderTest, createDptNegTest)
{
    auto dpt = DptWbMqttBuilder->Create(knx::object::TDatapointId{1000});
    EXPECT_EQ(dpt, nullptr);
}