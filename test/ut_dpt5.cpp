#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt5.h"
#include "gtest/gtest.h"

class Dpt5Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt5>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt5> Dpt;
};

TEST_F(Dpt5Test, InitVal)
{
    EXPECT_EQ(0, Dpt->ToMqtt().at(0).As<double>());
    std::vector<uint8_t> vec = {0x00, 0x00};
    EXPECT_EQ(vec, Dpt->ToKnx());
}

TEST_F(Dpt5Test, toMqttTest)
{
    EXPECT_NO_THROW(Dpt->FromKnx({0x00, 25}));
    EXPECT_EQ(25, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_NO_THROW(Dpt->FromKnx({0x00, 255}));
    EXPECT_EQ(255, Dpt->ToMqtt().at(0).As<double>());
}

TEST_F(Dpt5Test, toKnxTest)
{
    EXPECT_NO_THROW(Dpt->FromMqtt(0, 45.0));
    std::vector<uint8_t> vec = {0, 45};
    EXPECT_EQ(vec, Dpt->ToKnx());
    EXPECT_NO_THROW(Dpt->FromMqtt(0, 254.0));
    vec = {0, 254};
    EXPECT_EQ(vec, Dpt->ToKnx());
}

TEST_F(Dpt5Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt5Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}