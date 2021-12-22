#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt12.h"
#include "gtest/gtest.h"

class Dpt12Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt12>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt12> Dpt;
    std::vector<std::pair<uint32_t, std::vector<uint8_t>>> Samples = {{0, {0x00, 0x00, 0x00, 0x00, 0x00}},
                                                                      {4294967295U, {0x00, 0xFF, 0xFF, 0xFF, 0xFF}},
                                                                      {1147483647, {0x00, 0x44, 0x65, 0x35, 0xFF}},
                                                                      {254, {0x00, 0x00, 0x00, 0x00, 254}}};
};

TEST_F(Dpt12Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).first, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).second, Dpt->ToKnx());
}

TEST_F(Dpt12Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<double>());
    }
}

TEST_F(Dpt12Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.first)));
        EXPECT_EQ(sample.second, Dpt->ToKnx());
    }
}

TEST_F(Dpt12Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt12Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}