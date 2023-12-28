#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt4.h"
#include <gtest/gtest.h>

class Dpt4Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt4>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt4> Dpt;
};

TEST_F(Dpt4Test, InitVal)
{
    EXPECT_EQ("", Dpt->ToMqtt().at(0).As<std::string>());
    std::vector<uint8_t> vec = {0x00, 0x00};
    EXPECT_EQ(vec, Dpt->ToKnx());
}

TEST_F(Dpt4Test, toMqttTest)
{
    EXPECT_NO_THROW(Dpt->FromKnx({0x00, 'A'}));
    EXPECT_EQ("A", Dpt->ToMqtt().at(0).As<std::string>());
    EXPECT_NO_THROW(Dpt->FromKnx({0x00, 'F'}));
    EXPECT_EQ("F", Dpt->ToMqtt().at(0).As<std::string>());
}

TEST_F(Dpt4Test, toKnxTest)
{
    EXPECT_NO_THROW(Dpt->FromMqtt(0, "c"));
    std::vector<uint8_t> vec = {0, 'c'};
    EXPECT_EQ(vec, Dpt->ToKnx());
    EXPECT_NO_THROW(Dpt->FromMqtt(0, "p"));
    vec = {0, 'p'};
    EXPECT_EQ(vec, Dpt->ToKnx());
}

TEST_F(Dpt4Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt4Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, 26), std::exception);
}