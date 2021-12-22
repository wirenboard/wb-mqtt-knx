#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt8.h"
#include "gtest/gtest.h"

class Dpt8Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt8>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt8> Dpt;
    std::vector<std::pair<int32_t, std::vector<uint8_t>>> Samples = {{0, {0x00, 0x00, 0x00}},
                                                                     {32767, {0x00, 0x7F, 0xFF}},
                                                                     {12345, {0x00, 0x30, 0x39}},
                                                                     {-1, {0x00, 0xFF, 0xFF}},
                                                                     {-32768, {0x00, 0x80, 0x00}},
                                                                     {254, {0x00, 0x00, 254}}};
};

TEST_F(Dpt8Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).first, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).second, Dpt->ToKnx());
}

TEST_F(Dpt8Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<double>());
    }
}

TEST_F(Dpt8Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.first)));
        EXPECT_EQ(sample.second, Dpt->ToKnx());
    }
}

TEST_F(Dpt8Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt8Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}