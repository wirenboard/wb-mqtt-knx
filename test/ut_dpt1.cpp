#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt1.h"
#include "gtest/gtest.h"

class Dpt1Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt1>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt1> Dpt;
};

TEST_F(Dpt1Test, InitVal)
{
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(0x00, Dpt->ToKnx().at(0));
}

TEST_F(Dpt1Test, toMqttTest)
{
    EXPECT_TRUE(Dpt->FromKnx({0x00}));
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_TRUE(Dpt->FromKnx({0x01}));
    EXPECT_EQ(true, Dpt->ToMqtt().at(0).As<bool>());
}

TEST_F(Dpt1Test, toKnxTest)
{
    EXPECT_TRUE(Dpt->FromMqtt(0, false));
    EXPECT_EQ(0, Dpt->ToKnx().at(0));
    EXPECT_TRUE(Dpt->FromMqtt(0, true));
    EXPECT_EQ(1, Dpt->ToKnx().at(0));
}

TEST_F(Dpt1Test, fromKnxNegativeTest)
{
    EXPECT_FALSE(Dpt->FromKnx({0x00, 0x01}));
}

TEST_F(Dpt1Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, 23), std::exception);
}