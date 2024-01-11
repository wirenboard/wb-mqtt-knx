#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt2.h"
#include <gtest/gtest.h>

class Dpt2Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt2>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt2> Dpt;
};

TEST_F(Dpt2Test, InitVal)
{
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(false, Dpt->ToMqtt().at(1).As<bool>());

    EXPECT_EQ(std::vector<uint8_t>{0x00}, Dpt->ToKnx());
}

TEST_F(Dpt2Test, toMqttTest)
{
    EXPECT_NO_THROW(Dpt->FromKnx({0b10}));
    EXPECT_EQ(true, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(false, Dpt->ToMqtt().at(1).As<bool>());

    EXPECT_NO_THROW(Dpt->FromKnx({0b01}));
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(true, Dpt->ToMqtt().at(1).As<bool>());
}

TEST_F(Dpt2Test, toKnxTest)
{
    EXPECT_NO_THROW(Dpt->FromMqtt(0, true));
    EXPECT_NO_THROW(Dpt->FromMqtt(1, true));
    EXPECT_EQ(std::vector<uint8_t>{0b11}, Dpt->ToKnx());

    EXPECT_NO_THROW(Dpt->FromMqtt(0, false));
    EXPECT_NO_THROW(Dpt->FromMqtt(1, true));
    EXPECT_EQ(std::vector<uint8_t>{0b01}, Dpt->ToKnx());
}

TEST_F(Dpt2Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00, 0x00}), knx::TKnxException);
}

TEST_F(Dpt2Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}