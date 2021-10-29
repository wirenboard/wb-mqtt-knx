#include "../src/knxexception.h"
#include "../src/knxtelegram.h"
#include <gtest/gtest.h>
#include <string.h>

TEST(KnxTelegramTest, FromKnx0)
{
    std::vector<uint8_t> knxData = {0xbc, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28};

    knx::TTelegram telegram(knxData);

    EXPECT_EQ(telegram.GetSourceAddress(), 0x01);
    EXPECT_EQ(telegram.GetReceiverAddress(), 0x9737);
    EXPECT_EQ(telegram.IsGroupAddressed(), false);
    EXPECT_EQ(telegram.Tpdu().Size(), 5);
    EXPECT_EQ(telegram.Tpdu().GetAPCI(), knx::telegram::TApci::GroupValueWrite);
    EXPECT_EQ(telegram.Tpdu().GetSequenceNumber(), 0);
    EXPECT_EQ(telegram.Tpdu().GetCommunicationType(), knx::telegram::TCommunicationType::UDP);
    EXPECT_EQ(telegram.IsRepeated(), false);
    EXPECT_EQ(telegram.GetPriority(), knx::telegram::TPriority::Priority4_NormalMode_LowPriority);
}

TEST(KnxTelegramTest, FromKnx1)
{
    std::vector<uint8_t> knxData = {0xbc, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28};

    knx::TTelegram telegram(knxData);

    EXPECT_EQ(telegram.GetRawTelegram(), knxData);
}

TEST(KnxTelegramTest, FromKnx2)
{
    knx::TTelegram telegram2({0xB0, 0x11, 0x02, 0x11, 0x28, 0x60, 0x81, 0x84});
    EXPECT_EQ(telegram2.Tpdu().GetControlDataType(), knx::telegram::TControlDataType::Disconnect);
}

TEST(KnxTelegramTest, FromKnxThrow)
{
    std::vector<std::vector<uint8_t>> failedKnxDataSet = {
        {0xFF, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28},
        {0xbc, 0x00, 0x01, 0x97, 0x37, 0x74, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x23},
        {},
        {0xbc, 0x00, 0x01, 0x97, 0x37, 0x73, 0x00, 0x80, 0xcf, 0x0e, 0xff, 0x28},
        {0xbc}};

    uint32_t dataNum = 0;
    for (const auto& knxData: failedKnxDataSet) {
        EXPECT_THROW(knx::TTelegram telegram(knxData), knx::TKnxException) << "KnxDataIndex: " << dataNum;
        ++dataNum;
    }
}

TEST(KnxTelegramTest, FalseKnxApci)
{
    knx::TTelegram telegram;
    EXPECT_THROW(telegram.SetPriority(static_cast<knx::telegram::TPriority>(435)), knx::TKnxException);
    EXPECT_THROW(telegram.Tpdu().SetAPCI(static_cast<knx::telegram::TApci>(34)), knx::TKnxException);
}