#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt13.h"
#include <gtest/gtest.h>

class Dpt13Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt13>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt13> Dpt;
    std::vector<std::pair<int32_t, std::vector<uint8_t>>> Samples = {{0, {0x00, 0x00, 0x00, 0x00, 0x00}},
                                                                     {2147483647, {0x00, 0x7F, 0xFF, 0xFF, 0xFF}},
                                                                     {1147483647, {0x00, 0x44, 0x65, 0x35, 0xFF}},
                                                                     {-1134343442, {0x00, 0xBC, 0x63, 0x4A, 0xEE}},
                                                                     {-2147483648, {0x00, 0x80, 0x00, 0x00, 0x00}},
                                                                     {-1, {0x00, 0xFF, 0xFF, 0xFF, 0xFF}}};
};

TEST_F(Dpt13Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).first, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).second, Dpt->ToKnx());
}

TEST_F(Dpt13Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<double>());
    }
}

TEST_F(Dpt13Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.first)));
        EXPECT_EQ(sample.second, Dpt->ToKnx());
    }
}

TEST_F(Dpt13Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt13Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}