#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt10.h"
#include "gtest/gtest.h"

namespace
{
    struct TestSample
    {
        uint32_t Day;
        uint32_t Hour;
        uint32_t Minutes;
        uint32_t Seconds;
        std::vector<uint8_t> KnxPayload;
    };
}

class Dpt10Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt10>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt10> Dpt;
    std::vector<TestSample> Samples = {{0, 0, 0, 0, {0x00, 0x00, 0x00, 0x00}},
                                       {1, 2, 3, 4, {0x00, 1 << 5 | 2, 0x03, 0x04}},
                                       {7, 23, 59, 59, {0x00, 7 << 5 | 23, 59, 59}},
                                       {4, 12, 31, 35, {0x00, 4 << 5 | 12, 31, 35}}};
};

TEST_F(Dpt10Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).Day, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).Hour, Dpt->ToMqtt().at(1).As<double>());
    EXPECT_EQ(Samples.at(0).Minutes, Dpt->ToMqtt().at(2).As<double>());
    EXPECT_EQ(Samples.at(0).Seconds, Dpt->ToMqtt().at(3).As<double>());
    EXPECT_EQ(Samples.at(0).KnxPayload, Dpt->ToKnx());
}

TEST_F(Dpt10Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.KnxPayload));
        EXPECT_EQ(sample.Day, Dpt->ToMqtt().at(0).As<double>());
        EXPECT_EQ(sample.Hour, Dpt->ToMqtt().at(1).As<double>());
        EXPECT_EQ(sample.Minutes, Dpt->ToMqtt().at(2).As<double>());
        EXPECT_EQ(sample.Seconds, Dpt->ToMqtt().at(3).As<double>());
    }
}

TEST_F(Dpt10Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.Day)));
        EXPECT_NO_THROW(Dpt->FromMqtt(1, static_cast<double>(sample.Hour)));
        EXPECT_NO_THROW(Dpt->FromMqtt(2, static_cast<double>(sample.Minutes)));
        EXPECT_NO_THROW(Dpt->FromMqtt(3, static_cast<double>(sample.Seconds)));
        EXPECT_EQ(sample.KnxPayload, Dpt->ToKnx());
    }
}

TEST_F(Dpt10Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt10Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}