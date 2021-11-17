#include <gtest/gtest.h>

#include "../src/knxgroupaddress.h"

TEST(KnxGroupAddressTest, TrivialConstructorTest)
{
    uint32_t main = 1;
    uint32_t middle = 2;
    uint32_t sub = 102;
    knx::TKnxGroupAddress address(main, middle, sub);

    EXPECT_EQ(main, address.GetMainGroup());
    EXPECT_EQ(middle, address.GetMiddleGroup());
    EXPECT_EQ(sub, address.GetSubGroup());
}

TEST(KnxGroupAddressTest, ConstructorEibTest)
{
    uint32_t main = 1;
    uint32_t middle = 2;
    uint32_t sub = 102;
    knx::TKnxGroupAddress address(main, middle, sub);
    knx::TKnxGroupAddress address2(address.GetEibAddress());

    EXPECT_EQ(main, address2.GetMainGroup());
    EXPECT_EQ(middle, address2.GetMiddleGroup());
    EXPECT_EQ(sub, address2.GetSubGroup());
}

TEST(KnxGroupAddressTest, EqualOperatorTest)
{
    uint32_t main = 1;
    uint32_t middle = 2;
    uint32_t sub = 102;
    knx::TKnxGroupAddress address(main, middle, sub);
    knx::TKnxGroupAddress address2(address.GetEibAddress());

    EXPECT_EQ(address, address2);
}

TEST(KnxGroupAddressTest, LessOperatorTest)
{
    uint32_t main = 1;
    uint32_t middle = 2;
    uint32_t sub = 102;
    knx::TKnxGroupAddress address(main, middle, sub);
    knx::TKnxGroupAddress address2(main, middle, sub + 1);

    EXPECT_LT(address, address2);
}