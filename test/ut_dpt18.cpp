#include "../src/knxexception.h"
#include "../src/knxgroupobject/dpt18.h"
#include "gtest/gtest.h"

namespace
{
    struct TestSample
    {
        bool Control;
        uint32_t SceneNumber;
        std::vector<uint8_t> KnxPayload;
    };
}

class Dpt18Test: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDpt18>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDpt18> Dpt;
    std::vector<TestSample> Samples = {{false, 0, {0x00, 0x00}},
                                       {true, 1, {0x00, 1 << 7 | 1}},
                                       {true, 63, {0x00, 1 << 7 | 63}},
                                       {false, 31, {0x00, 31}}};
};

TEST_F(Dpt18Test, InitVal)
{
    EXPECT_EQ(Samples.at(0).Control, Dpt->ToMqtt().at(0).As<bool>());
    EXPECT_EQ(Samples.at(0).SceneNumber, Dpt->ToMqtt().at(1).As<double>());
    EXPECT_EQ(Samples.at(0).KnxPayload, Dpt->ToKnx());
}

TEST_F(Dpt18Test, toMqttTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromKnx(sample.KnxPayload));
        EXPECT_EQ(sample.Control, Dpt->ToMqtt().at(0).As<bool>());
        EXPECT_EQ(sample.SceneNumber, Dpt->ToMqtt().at(1).As<double>());
    }
}

TEST_F(Dpt18Test, toKnxTest)
{
    for (const auto& sample: Samples) {
        EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<bool>(sample.Control)));
        EXPECT_NO_THROW(Dpt->FromMqtt(1, static_cast<double>(sample.SceneNumber)));
        EXPECT_EQ(sample.KnxPayload, Dpt->ToKnx());
    }
}

TEST_F(Dpt18Test, fromKnxNegativeTest)
{
    EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
}

TEST_F(Dpt18Test, formMqttNegativeTest)
{
    EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
}