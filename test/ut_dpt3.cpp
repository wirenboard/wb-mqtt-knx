#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt3.h"
#include <gtest/gtest.h>

class Dpt3Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt3>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt3> Dpt;
};

TEST_F(Dpt3Test, InitVal)
{
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(0.0, Dpt->ToMqtt().at(1).As<double>());

    EXPECT_EQ(std::vector<uint8_t>{0x00}, Dpt->ToKnx());
}

TEST_F(Dpt3Test, toMqttTest)
{
    EXPECT_NO_THROW(Dpt->FromKnx({0b1101}));
    EXPECT_EQ(true, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(5.0, Dpt->ToMqtt().at(1).As<double>());

    EXPECT_NO_THROW(Dpt->FromKnx({0b0011}));
    EXPECT_EQ(false, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(3.0, Dpt->ToMqtt().at(1).As<double>());
}

TEST_F(Dpt3Test, toKnxTest)
{
    EXPECT_NO_THROW(Dpt->FromMqtt(0, true));
    EXPECT_NO_THROW(Dpt->FromMqtt(1, 5.0));
    EXPECT_EQ(std::vector<uint8_t>{0b1101}, Dpt->ToKnx());

    EXPECT_NO_THROW(Dpt->FromMqtt(0, false));
    EXPECT_NO_THROW(Dpt->FromMqtt(1, 3.0));
    EXPECT_EQ(std::vector<uint8_t>{0b0011}, Dpt->ToKnx());
}

TEST_F(Dpt3Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00, 0x00}), knx::TKnxException);
}

TEST_F(Dpt3Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}