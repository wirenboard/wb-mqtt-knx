#include <gtest/gtest.h>

#include "../src/knxindividualaddress.h"

TEST(KnxIndividualAddressTest, TrivialConstructorTest)
{
    uint32_t area = 1;
    uint32_t line = 2;
    uint32_t device = 102;
    knx::TKnxIndividualAddress address(area, line, device);

    EXPECT_EQ(area, address.GetArea());
    EXPECT_EQ(line, address.GetLine());
    EXPECT_EQ(device, address.GetDeviceAddress());
}

TEST(KnxIndividualAddressTest, ConstructorEibTest)
{
    uint32_t area = 1;
    uint32_t line = 2;
    uint32_t device = 102;
    knx::TKnxIndividualAddress address(area, line, device);
    knx::TKnxIndividualAddress address2(address.GetEibAddress());

    EXPECT_EQ(area, address2.GetArea());
    EXPECT_EQ(line, address2.GetLine());
    EXPECT_EQ(device, address2.GetDeviceAddress());
}

TEST(KnxIndividualAddressTest, EqualOperatorTest)
{
    uint32_t area = 1;
    uint32_t line = 7;
    uint32_t device = 102;
    knx::TKnxIndividualAddress address(area, line, device);
    knx::TKnxIndividualAddress address2(address.GetEibAddress());

    EXPECT_EQ(address, address2);
}

TEST(KnxIndividualAddressTest, LessOperatorTest)
{
    uint32_t area = 1;
    uint32_t line = 2;
    uint32_t device = 102;
    knx::TKnxIndividualAddress address(area, line, device);
    knx::TKnxIndividualAddress address2(area, line, device + 1);

    EXPECT_LT(address, address2);
}

TEST(KnxIndividualAddressTest, ToStringTest)
{
    knx::TKnxIndividualAddress address(1, 2, 3);
    EXPECT_EQ("1.2.3", address.ToString());
    EXPECT_EQ("1/2/3", address.ToString('/'));
}

TEST(KnxIndividualAddressTest, FromStringTest)
{

    knx::TKnxIndividualAddress address{"1/2/3", '/'};
    EXPECT_EQ("1/2/3", address.ToString('/'));

    address = knx::TKnxIndividualAddress{"0.0.0"};
    EXPECT_EQ("0.0.0", address.ToString());

    address = knx::TKnxIndividualAddress{"15.7.255"};
    EXPECT_EQ("15.7.255", address.ToString());
}

TEST(KnxIndividualAddressTest, FromStringNegativeTest)
{
    std::vector<std::string> addressList = {"a1.2.3", "32.2.3", "1.17.3", "1.1.256", "", "...", "65536"};

    for (const auto& addtessStr: addressList) {
        EXPECT_THROW(knx::TKnxIndividualAddress{addtessStr}, std::exception);
    }
}