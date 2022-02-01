#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt17.h"
#include "gtest/gtest.h"

namespace
{
    struct TestSample
    {
        uint32_t SceneNumber;
        std::vector<uint8_t> KnxPayload;
    };
}

class Dpt17Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt17>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt17> Dpt;
    std::vector<TestSample> Samples = {{0, {0x00, 0x00}}, {1, {0x00, 1}}, {63, {0x00, 63}}, {31, {0x00, 31}}};
};

TEST_F(Dpt17Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).SceneNumber, Dpt->ToMqtt().at(0).As<double>());
    EXPECT_EQ(Samples.at(0).KnxPayload, Dpt->ToKnx());
}

TEST_F(Dpt17Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.KnxPayload));
        EXPECT_EQ(sample.SceneNumber, Dpt->ToMqtt().at(0).As<double>());
    }
}

TEST_F(Dpt17Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.SceneNumber)));
        EXPECT_EQ(sample.KnxPayload, Dpt->ToKnx());
    }
}

TEST_F(Dpt17Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt17Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}