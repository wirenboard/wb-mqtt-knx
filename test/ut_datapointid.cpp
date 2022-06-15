#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjsonbuilder.h"
#include "config.h"
#include "gtest/gtest.h"

class DatapointIdTest: public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

TEST_F(DatapointIdTest, mainIdTest)
{
    knx::object::TDatapointId datapointId(258);
    EXPECT_EQ(false, datapointId.HasSubId());
    EXPECT_EQ(258, datapointId.GetMain());
    EXPECT_EQ("258.xxx", datapointId.ToString());
}

TEST_F(DatapointIdTest, mainSubIdTest)
{
    knx::object::TDatapointId datapointId(221, 32);
    EXPECT_EQ(true, datapointId.HasSubId());
    EXPECT_EQ(221, datapointId.GetMain());
    EXPECT_EQ(32, datapointId.GetSub());
    EXPECT_EQ("221.032", datapointId.ToString());
}

TEST_F(DatapointIdTest, fromStringTest)
{
    knx::object::TDatapointId datapointId;
    EXPECT_EQ(true, datapointId.SetFromString("223.xx_dsfsdf"));
    EXPECT_EQ(false, datapointId.HasSubId());
    EXPECT_EQ(223, datapointId.GetMain());

    EXPECT_EQ(true, datapointId.SetFromString("224.21_3dd2!"));
    EXPECT_EQ(true, datapointId.HasSubId());
    EXPECT_EQ(224, datapointId.GetMain());
    EXPECT_EQ(21, datapointId.GetSub());
}

TEST_F(DatapointIdTest, fromStringNegTest)
{
    knx::object::TDatapointId datapointId;
    EXPECT_EQ(false, datapointId.SetFromString("223._dsfsdf"));
    EXPECT_EQ(false, datapointId.SetFromString("._dewfsdf"));
    EXPECT_EQ(false, datapointId.SetFromString(""));
    EXPECT_EQ(false, datapointId.SetFromString(".eds_dsfsdf"));
}

TEST_F(DatapointIdTest, EqTest)
{
    knx::object::TDatapointId a{12};
    knx::object::TDatapointId b{12};

    EXPECT_TRUE(a == b);
    b.SetMain(21);
    EXPECT_FALSE(a == b);
    b.SetMain(12);
    b.SetSub(2);
    EXPECT_FALSE(a == b);
    a.SetSub(2);
    EXPECT_TRUE(a == b);
    a.SetSub(4);
    EXPECT_FALSE(a == b);
}

TEST_F(DatapointIdTest, LessTest)
{
    knx::object::TDatapointId a{12};
    knx::object::TDatapointId b{12, 10};
    EXPECT_TRUE(a < b);
    a.SetSub(9);
    EXPECT_TRUE(a < b);
    b.SetMain(10);
    EXPECT_FALSE(a < b);
}