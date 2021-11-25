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
    uint32_t middle = 7;
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

TEST(KnxGroupAddressTest, ToStringTest)
{
    knx::TKnxGroupAddress address(1, 2, 3);
    EXPECT_EQ("1/2/3", address.ToString());
}

TEST(KnxGroupAddressTest, FromStringTest)
{

    knx::TKnxGroupAddress address{"1/2/3"};
    EXPECT_EQ("1/2/3", address.ToString());

    address = knx::TKnxGroupAddress{"0/0/0"};
    EXPECT_EQ("0/0/0", address.ToString());

    address = knx::TKnxGroupAddress{"15/7/255"};
    EXPECT_EQ("15/7/255", address.ToString());

    address = knx::TKnxGroupAddress{"15/2047"};
    EXPECT_EQ("15/7/255", address.ToString());
}

TEST(KnxGroupAddressTest, FromStringNegativeTest)
{
    std::vector<std::string> addressList = {"a1/2/3", "16/2/3", "1/8/3", "1/1/256", "", "///"};

    for (const auto& addtessStr: addressList) {
        EXPECT_THROW(knx::TKnxGroupAddress{addtessStr}, std::exception);
    }
}