#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjsonbuilder.h"
#include "config.h"
#include "testutils.h"
#include "gtest/gtest.h"

class DptJsonBuilderTest: public ::testing::Test
{
protected:
    std::string SourceDir = CMAKE_SOURCE_DIR;
    void SetUp() override
    {
        JsonDptBuilder = std::make_unique<knx::object::TDptJsonBuilder>(SourceDir + "/wb-mqtt-knx-jsondpt.conf", "");
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptJsonBuilder> JsonDptBuilder;
};

TEST_F(DptJsonBuilderTest, createB1DptTest)
{
    auto dptB1 = JsonDptBuilder->Create(knx::object::TDatapointId(1));
    dptB1->FromKnx({0x01});
    auto jsonStr = dptB1->ToMqtt().at(0).As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(true, jsonObject["b"].asBool());
}

TEST_F(DptJsonBuilderTest, createB1Sub2DptTest)
{
    auto dptB1 = JsonDptBuilder->Create(knx::object::TDatapointId(1, 2));
    dptB1->FromKnx({0x01});
    auto jsonStr = dptB1->ToMqtt().at(0).As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(true, jsonObject["bool"].asBool());
}

TEST_F(DptJsonBuilderTest, createB2DptTest)
{
    auto dptB2 = JsonDptBuilder->Create(knx::object::TDatapointId(2));
    dptB2->FromKnx({0x02});
    auto jsonStr = dptB2->ToMqtt().at(0).As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(true, jsonObject["c"].asBool());
    EXPECT_EQ(false, jsonObject["v"].asBool());
}

TEST_F(DptJsonBuilderTest, createTimeOfDayDptTest)
{
    auto dptTime = JsonDptBuilder->Create(knx::object::TDatapointId(10));
    dptTime->FromKnx({0x00, (1 << 5) | 21, 35, 19});
    auto jsonStr = dptTime->ToMqtt().at(0).As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(1, jsonObject["Day"].asUInt());
    EXPECT_EQ(21, jsonObject["Hour"].asUInt());
    EXPECT_EQ(35, jsonObject["Minutes"].asUInt());
    EXPECT_EQ(19, jsonObject["Seconds"].asUInt());
}