#include "knxtelegram.h"
#include "knxexception.h"
#include "wblib/utils.h"
#include <bitset>

using namespace knx;

namespace
{
    constexpr auto POSITION_CONTROL_FIELD      = 0;
    constexpr auto POSITION_SOURCE_ADDRESS_H   = 1;
    constexpr auto POSITION_SOURCE_ADDRESS_L   = 2;
    constexpr auto POSITION_RECEIVER_ADDRESS_H = 3;
    constexpr auto POSITION_RECEIVER_ADDRESS_L = 4;
    constexpr auto POSITION_NPDU_FIRST         = 5;
    constexpr auto POSITION_TPDU_FIRST         = 6;
    constexpr auto POSITION_APDU_FIRST         = 7;
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

uint8_t TTelegram::GetAPDULength() const
{
    return ApduPayload.size();
}

telegram::TTypeOfTransportLayer TTelegram::GetTypeOfTransportLayer() const
{
    return TypeOfTransportLayer;
}

void TTelegram::SetTypeOfTransportLayer(telegram::TTypeOfTransportLayer type)
{
    if (type != telegram::TTypeOfTransportLayer::UDP && type != telegram::TTypeOfTransportLayer::NDP)
        wb_throw(TKnxException, "Unsupported type of transport layer");
    TypeOfTransportLayer = type;
}

uint8_t TTelegram::GetSequenceNumber() const
{
    return SequenceNumber;
}

void TTelegram::SetSequenceNumber(uint8_t number)
{
    SequenceNumber = number;
}

telegram::TApci TTelegram::GetAPCI() const
{
    return Apci;
}

void TTelegram::SetAPCI(telegram::TApci apci)
{
    if (apci > telegram::TApci::Escape)
        wb_throw(TKnxException, "Incorrect APCI");
    Apci = apci;
}

std::vector<uint8_t> TTelegram::GetTPDUPayload() const
{
    std::vector<uint8_t> telegram(ApduPayload.size() + 1, 0);

    telegram[0] &= ~(0x3 << 6);
    telegram[0] |= static_cast<uint8_t>(GetTypeOfTransportLayer()) << 6;

    telegram[0] &= ~(0x0F << 2);
    telegram[0] |= (GetSequenceNumber() & 0x0F) << 2;

    telegram[0] &= ~(0x03);
    telegram[0] |= (static_cast<uint8_t>(GetAPCI()) >> 2) & 0x03;

    telegram[1] = ((static_cast<uint8_t>(GetAPCI()) & 0x03) << 6) | (ApduPayload[0] & 0x3F);

    std::copy(ApduPayload.begin() + 1, ApduPayload.end(), telegram.begin() + 2);

    return telegram;
}

std::vector<uint8_t> TTelegram::GetAPDUPayload() const
{
    return ApduPayload;
}

void TTelegram::SetAPDUPayload(const std::vector<uint8_t>& payload)
{
    if (payload.empty() || (payload.size() > TTelegram::MaxPayloadSize))
        wb_throw(TKnxException, "Incorrect payload size");
    ApduPayload = payload;
    ApduPayload[0] &= 0x3F;
}

std::vector<uint8_t> TTelegram::GetRawTelegram() const
{
    std::vector<uint8_t> telegram(SizeWithoutPayload - 2, 0);

    telegram[POSITION_CONTROL_FIELD] = ((static_cast<uint8_t>(Priority) & 0x3) << 2) |
                                       (!IsRepeatedTelegram ? (1 << 5) : 0) | (1 << 7) | (1 << 4);

    telegram[POSITION_SOURCE_ADDRESS_H] = (GetSourceAddress() >> 8) & 0xFF;
    telegram[POSITION_SOURCE_ADDRESS_L] = GetSourceAddress() & 0xFF;

    telegram[POSITION_RECEIVER_ADDRESS_H] = (GetReceiverAddress() >> 8) & 0xFF;
    telegram[POSITION_RECEIVER_ADDRESS_L] = GetReceiverAddress() & 0xFF;

    telegram[POSITION_NPDU_FIRST] = IsGroupAddressed() ? telegram[POSITION_NPDU_FIRST] | (1 << 7)
                                                       : telegram[POSITION_NPDU_FIRST] & ~(1 << 7);

    telegram[POSITION_NPDU_FIRST] &= ~(0x7 << 4);
    telegram[POSITION_NPDU_FIRST] |= (GetRoutingCounter() & 0x7) << 4;

    telegram[POSITION_NPDU_FIRST] &= ~(0x0F);
    telegram[POSITION_NPDU_FIRST] |= GetAPDULength();

    auto tpdu = GetTPDUPayload();
    std::copy(tpdu.begin(), tpdu.end(), std::back_inserter(telegram));

    telegram.push_back(CalculateParity(telegram));
    return telegram;
}

bool TTelegram::SetRawTelegram(const std::vector<uint8_t>& knxBuffer)
{
    const auto knxBufferSize = knxBuffer.size();

    if ((knxBufferSize <= SizeWithoutPayload) || (knxBufferSize > (SizeWithoutPayload + MaxPayloadSize))){
        wb_throw(TKnxException, "Invalid KNX raw data length");
    } else if (knxBufferSize != (SizeWithoutPayload + (knxBuffer[POSITION_NPDU_FIRST] & 0x0F))){
        wb_throw(TKnxException, "Wrong APDU payload length");
    }

    if ((knxBuffer[POSITION_CONTROL_FIELD] & ~(1<<5 | 1<<3 | 1<<2)) != (1<<7 | 1<<4)){
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

    SetTypeOfTransportLayer(
        static_cast<telegram::TTypeOfTransportLayer>(knxBuffer[POSITION_TPDU_FIRST] >> 6));

    SetSequenceNumber((knxBuffer[POSITION_TPDU_FIRST] >> 2) & 0x0F);

    SetAPCI(static_cast<telegram::TApci>(((knxBuffer[POSITION_TPDU_FIRST] & 0x03) << 2) |
                                         (knxBuffer[POSITION_APDU_FIRST] >> 6)));

    std::vector<uint8_t> payload(knxBuffer.begin() + POSITION_APDU_FIRST, knxBuffer.end() - 1);

    SetAPDUPayload(payload);

    return true;
}

uint8_t TTelegram::CalculateParity(const std::vector<uint8_t>& data)
{
    uint8_t parity = 0;
    for (const auto& val : data) {
        parity ^= val;
    }
    return ~parity;
}
