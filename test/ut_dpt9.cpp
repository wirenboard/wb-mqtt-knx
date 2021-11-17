
#include "../src/knxgroupobject/dpt9.h"
#include "gtest/gtest.h"

class Dpt9Test: public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}

    std::vector<std::pair<double, std::vector<uint8_t>>> TestSamples = {{36.66, {0, 0x0F, 0x29}},
                                                                        {27.12, {0, 0x0D, 0x4C}},
                                                                        {27.52, {0, 0xD, 0x60}},
                                                                        {28.06, {0, 0x0D, 0x7B}},
                                                                        {40.6, {0, 0x0F, 0xEE}}};
};

TEST_F(Dpt9Test, toMqttTest)
{

    knx::object::TDpt9 dpt;

    for (const auto& sample: TestSamples) {
        std::vector<WBMQTT::TAny> mqttData = dpt.knxToMqtt(sample.second);
        EXPECT_EQ(1, mqttData.size());
        auto floatValue = (mqttData[0]).As<double>();
        EXPECT_NEAR(sample.first, floatValue, 0.01) << std::to_string(floatValue);
    }
}

TEST_F(Dpt9Test, toKnxTest)
{
    knx::object::TDpt9 dpt;

    for (const auto& sample: TestSamples) {
        auto knxData = dpt.mqttToKnx(0, sample.first);
        EXPECT_EQ(sample.second, knxData);
    }
}