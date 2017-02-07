#include "../src/Telegram.h"
#include <gtest/gtest.h>
#include <string.h>

TEST(TelegramTest, FromKnx1)
{
    uint8_t telegram[] = {0xbc, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28};

    TKnxTelegram t(telegram, sizeof(telegram));

    EXPECT_TRUE(t.GetMqttPayload() == "i:0/0/1 i:9/7/55 GroupValueWrite 0x00 0xcf 0x0e 0xff ");
    EXPECT_TRUE(t.IsGroup() == false);
    EXPECT_TRUE(t.GetSrcAddr() == 0x1);
    EXPECT_TRUE(t.GetDstAddr() == 0x9737);
    EXPECT_TRUE(t.GetSize() == 5);
}

TEST(TelegramTest, FromKnx2)
{
    uint8_t telegram[] = {0xbc, 0x00, 0x00, 0x4f, 0x37, 0xf1, 0x00, 0x00, 0xca};

    TKnxTelegram t(telegram, sizeof(telegram));

    EXPECT_TRUE(t.GetMqttPayload() == "i:0/0/0 g:9/7/55 GroupValueRead 0x00 ");
    EXPECT_TRUE(t.IsGroup() == true);
    EXPECT_TRUE(t.GetSrcAddr() == 0x0);
    EXPECT_TRUE(t.GetDstAddr() == 0x4f37);
    EXPECT_TRUE(t.GetSize() == 2);
}

TEST(TelegramTest, FromMqtt1)
{
    std::string payload = "i:0/0/1:9/7/55 GroupValueWrite 0x0 0xcf 14 0xff";

    TKnxTelegram t(payload);

    uint8_t telegram[] = {0x00, 0x80, 0xcf, 0x0e, 0xff};
    EXPECT_TRUE(t.IsGroup() == false);
    EXPECT_TRUE(t.GetSrcAddr() == 0x1);
    EXPECT_TRUE(t.GetDstAddr() == 0x9737);
    EXPECT_TRUE(t.GetSize() == 5);
    EXPECT_FALSE(memcmp(t.GetKnxPayload(), telegram, sizeof(telegram)));
}

TEST(TelegramTest, FromMqtt2)
{
    std::string payload = "g:9/7/55 GroupValueRead";

    TKnxTelegram t(payload);

    uint8_t telegram[] = {0x00, 0x00};
    EXPECT_TRUE(t.IsGroup() == true);
    EXPECT_TRUE(t.GetSrcAddr() == 0x0);
    EXPECT_TRUE(t.GetDstAddr() == 0x4f37);
    EXPECT_TRUE(t.GetSize() == 2);
    EXPECT_FALSE(memcmp(t.GetKnxPayload(), telegram, sizeof(telegram)));
}
