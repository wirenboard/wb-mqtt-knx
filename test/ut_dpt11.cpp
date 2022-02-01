#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt11.h"
#include "gtest/gtest.h"

namespace
{
    struct TestSample
    {
        uint32_t Day;
        uint32_t Month;
        uint32_t Year;
        std::vector<uint8_t> KnxPayload;
    };
}

class Dpt11Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt11>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt11> Dpt;
    std::vector<TestSample> Samples = {{1, 1, 0, {0x00, 0x01, 0x01, 0x00}},
                                       {1, 2, 3, {0x00, 1, 2, 3}},
                                       {31, 12, 99, {0x00, 31, 12, 99}},
                                       {16, 6, 53, {0x00, 16, 6, 53}}};
};

TEST_F(Dpt11Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).Day, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).Month, Dpt->ToMqtt().at(1).As<double>());
    EXPECT_EQ(Samples.at(0).Year, Dpt->ToMqtt().at(2).As<double>());
    EXPECT_EQ(Samples.at(0).KnxPayload, Dpt->ToKnx());
}

TEST_F(Dpt11Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.KnxPayload));
        EXPECT_EQ(sample.Day, Dpt->ToMqtt().at(0).As<double>());
        EXPECT_EQ(sample.Month, Dpt->ToMqtt().at(1).As<double>());
        EXPECT_EQ(sample.Year, Dpt->ToMqtt().at(2).As<double>());
    }
}

TEST_F(Dpt11Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.Day)));
        EXPECT_NO_THROW(Dpt->FromMqtt(1, static_cast<double>(sample.Month)));
        EXPECT_NO_THROW(Dpt->FromMqtt(2, static_cast<double>(sample.Year)));
        EXPECT_EQ(sample.KnxPayload, Dpt->ToKnx());
    }
}

TEST_F(Dpt11Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt11Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}