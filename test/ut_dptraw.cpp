#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptraw.h"
#include "gtest/gtest.h"

class DptRawTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDptRaw>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptRaw> Dpt;
};

TEST_F(DptRawTest, InitVal)
{
    EXPECT_EQ("0x00 ", Dpt->ToMqtt().at(0).As<std::string>());
    EXPECT_EQ(0x00, Dpt->ToKnx().at(0));
}

TEST_F(DptRawTest, toMqttTest)
{
    EXPECT_TRUE(Dpt->FromKnx({0x00, 0xAA}));
    EXPECT_EQ("0x00 0xaa ", Dpt->ToMqtt().at(0).As<std::string>());
    EXPECT_TRUE(Dpt->FromKnx({0x01}));
    EXPECT_EQ("0x01 ", Dpt->ToMqtt().at(0).As<std::string>());
}

TEST_F(DptRawTest, toKnxTest)
{

    EXPECT_TRUE(Dpt->FromMqtt(0, "0x01 0xbb 0b11"));
    std::vector<uint8_t> vec = {1, 0xBB, 0x03};
    EXPECT_EQ(vec, Dpt->ToKnx());
    EXPECT_TRUE(Dpt->FromMqtt(0, "0xAA"));
    vec = {0xAA};
    EXPECT_EQ(vec, Dpt->ToKnx());
}

TEST_F(DptRawTest, fromKnxNegativeTest)
{
    EXPECT_FALSE(Dpt->FromKnx({}));
}

TEST_F(DptRawTest, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, 23), std::exception);
}
