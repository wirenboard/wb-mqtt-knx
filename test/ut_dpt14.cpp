#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt14.h"
#include <gtest/gtest.h>

class Dpt14Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt14>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt14> Dpt;
    std::vector<std::pair<float, std::vector<uint8_t>>> Samples = {{0, {0x00, 0x00, 0x00, 0x00, 0x00}},
                                                                   {2147483647.0, {0x00, 0x4F, 0x00, 0x00, 0x00}},
                                                                   {1147483647.0, {0x00, 0x4E, 0x88, 0xCA, 0x6C}},
                                                                   {-1134343442.0, {0x00, 0xCE, 0x87, 0x39, 0x6A}},
                                                                   {-2147483648.0, {0x00, 0xCF, 0x00, 0x00, 0x00}},
                                                                   {-1.0, {0x00, 0xBF, 0x80, 0x00, 0x00}},
                                                                   {-0.0012346, {0x00, 0xBA, 0xA1, 0xD2, 0x4D}}};
};

TEST_F(Dpt14Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).first, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).second, Dpt->ToKnx());
}

TEST_F(Dpt14Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<double>());
    }
}

TEST_F(Dpt14Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.first)));
        EXPECT_EQ(sample.second, Dpt->ToKnx());
    }
}

TEST_F(Dpt14Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt14Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}