#include "knxtelegram.h"
#include "knxexception.h"
#include "wblib/utils.h"
#include <bitset>

using namespace knx;

namespace
{
    constexpr auto POSITION_CONTROL_FIELD = 0;
    constexpr auto POSITION_SOURCE_ADDRESS_H = 1;
    constexpr auto POSITION_SOURCE_ADDRESS_L = 2;
    constexpr auto POSITION_RECEIVER_ADDRESS_H = 3;
    constexpr auto POSITION_RECEIVER_ADDRESS_L = 4;
    constexpr auto POSITION_NPDU_FIRST = 5;
} // namespace

TTelegram::TTelegram(const std::vector<uint8_t>& knxBuffer)
{
    SetRawTelegram(knxBuffer);
}

void TTelegram::SetRepeated(bool isRepeated)
{
    IsRepeatedTelegram = isRepeated;
}

bool TTelegram::IsRepeated() const
{
    return IsRepeatedTelegram;
}

void TTelegram::SetPriority(telegram::TPriority priority)
{
    if (priority > telegram::TPriority::Priority4_NormalMode_LowPriority) {
        wb_throw(TKnxException, "Incorrect priority");
    }
    Priority = priority;
}

telegram::TPriority TTelegram::GetPriority() const
{
    return Priority;
}

eibaddr_t TTelegram::GetSourceAddress() const
{
    return SourceAddress;
}

void TTelegram::SetSourceAddress(eibaddr_t addr)
{
    SourceAddress = addr;
}

eibaddr_t TTelegram::GetReceiverAddress() const
{
    return ReceiverAddress;
}

void TTelegram::SetReceiverAddress(eibaddr_t addr)
{
    ReceiverAddress = addr;
}

bool TTelegram::IsGroupAddressed() const
{
    return IsReceiverAddressGrouped;
}

void TTelegram::SetGroupAddressedFlag(bool groupAddressed)
{
    IsReceiverAddressGrouped = groupAddressed;
}

uint8_t TTelegram::GetRoutingCounter() const
{
    return RoutingCounter;
}

void TTelegram::SetRoutingCounter(uint8_t counter)
{
    RoutingCounter = counter;
}

std::vector<uint8_t> TTelegram::GetRawTelegram() const
{
    std::vector<uint8_t> telegram(SizeWithoutPayload - 2, 0);

    telegram[POSITION_CONTROL_FIELD] =
        ((static_cast<uint8_t>(Priority) & 0x3) << 2) | (!IsRepeatedTelegram ? (1 << 5) : 0) | (1 << 7) | (1 << 4);

    telegram[POSITION_SOURCE_ADDRESS_H] = (GetSourceAddress() >> 8) & 0xFF;
    telegram[POSITION_SOURCE_ADDRESS_L] = GetSourceAddress() & 0xFF;

    telegram[POSITION_RECEIVER_ADDRESS_H] = (GetReceiverAddress() >> 8) & 0xFF;
    telegram[POSITION_RECEIVER_ADDRESS_L] = GetReceiverAddress() & 0xFF;

    telegram[POSITION_NPDU_FIRST] =
        IsGroupAddressed() ? telegram[POSITION_NPDU_FIRST] | (1 << 7) : telegram[POSITION_NPDU_FIRST] & ~(1 << 7);

    telegram[POSITION_NPDU_FIRST] &= ~(0x7 << 4);
    telegram[POSITION_NPDU_FIRST] |= (GetRoutingCounter() & 0x7) << 4;

    telegram[POSITION_NPDU_FIRST] &= ~(0x0F);
    telegram[POSITION_NPDU_FIRST] |= Mtpdu.GetPayloadLength();

    auto tpdu = Mtpdu.GetRaw();
    std::copy(tpdu.begin(), tpdu.end(), std::back_inserter(telegram));

    telegram.push_back(CalculateParity(telegram));
    return telegram;
}

void TTelegram::SetRawTelegram(const std::vector<uint8_t>& knxBuffer)
{
    const auto knxBufferSize = knxBuffer.size();

    if ((knxBufferSize < SizeWithoutPayload) || (knxBufferSize > (SizeWithoutPayload + knx::TTpdu::MaxPayloadSize))) {
        wb_throw(TKnxException, "Invalid KNX raw data length");
    } else if (knxBufferSize != (SizeWithoutPayload + (knxBuffer[POSITION_NPDU_FIRST] & 0x0F))) {
        wb_throw(TKnxException, "Wrong APDU payload length");
    }

    if ((knxBuffer[POSITION_CONTROL_FIELD] & ~(1 << 5 | 1 << 3 | 1 << 2)) != (1 << 7 | 1 << 4)) {
        wb_throw(TKnxException, "Incorrect control field");
    }

    if (CalculateParity(knxBuffer) != 0) {
        wb_throw(TKnxException, "The parity field does not match");
    }

    SetRepeated((knxBuffer[POSITION_CONTROL_FIELD] & (1 << 5)) == 0);

    SetPriority(static_cast<telegram::TPriority>((knxBuffer[POSITION_CONTROL_FIELD] >> 2) & 0x03));

    SetSourceAddress((static_cast<eibaddr_t>(knxBuffer[POSITION_SOURCE_ADDRESS_H]) << 8) |
                     knxBuffer[POSITION_SOURCE_ADDRESS_L]);

    SetReceiverAddress((static_cast<eibaddr_t>(knxBuffer[POSITION_RECEIVER_ADDRESS_H]) << 8) |
                       knxBuffer[POSITION_RECEIVER_ADDRESS_L]);

    SetGroupAddressedFlag((knxBuffer[POSITION_NPDU_FIRST] & (1 << 7)) != 0);

    SetRoutingCounter((knxBuffer[POSITION_NPDU_FIRST] & ~(1 << 7)) >> 4);

    Mtpdu.SetRaw(knxBuffer.cbegin() + POSITION_NPDU_FIRST + 1, knxBuffer.cend() - 1);
}

uint8_t TTelegram::CalculateParity(const std::vector<uint8_t>& data)
{
    uint8_t parity = 0;
    for (const auto& val: data) {
        parity ^= val;
    }
    return ~parity;
}

const TTpdu& TTelegram::Tpdu() const
{
    return Mtpdu;
}

TTpdu& TTelegram::Tpdu()
{
    return Mtpdu;
}