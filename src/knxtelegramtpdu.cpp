#include "knxtelegramtpdu.h"
#include "knxexception.h"
#include "wblib/utils.h"

using namespace knx;

uint8_t TTpdu::GetPayloadLength() const
{
    return ApduPayload.size();
}

telegram::TCommunicationType TTpdu::GetCommunicationType() const
{
    return CommunicationType;
}

void TTpdu::SetCommunicationType(telegram::TCommunicationType type)
{
    if (type > telegram::TCommunicationType::NCD)
        wb_throw(TKnxException, "Incorrect type of transport layer");
    CommunicationType = type;
}

uint8_t TTpdu::GetSequenceNumber() const
{
    return SequenceNumber;
}

void TTpdu::SetSequenceNumber(uint8_t number)
{
    SequenceNumber = number;
}

telegram::TApci TTpdu::GetAPCI() const
{
    return Apci;
}

void TTpdu::SetAPCI(telegram::TApci apci)
{
    if (apci > telegram::TApci::Escape)
        wb_throw(TKnxException, "Incorrect APCI");
    Apci = apci;
}

std::vector<uint8_t> TTpdu::GetPayload() const
{
    return ApduPayload;
}

void TTpdu::SetPayload(const std::vector<uint8_t>& payload)
{
    SetPayload(payload.cbegin(), payload.cend());
}

void TTpdu::SetPayload(std::vector<uint8_t>::const_iterator beginIt, std::vector<uint8_t>::const_iterator endIt)
{
    uint32_t payloadSize = std::distance(beginIt, endIt);
    if (payloadSize <= MaxPayloadSize && payloadSize > 0) {
        ApduPayload.assign(beginIt, endIt);
        ApduPayload.front() &= 0x3F;
    } else {
        wb_throw(TKnxException, "Incorrect payload size");
    }
}

telegram::TControlDataType TTpdu::GetControlDataType() const
{
    return CDataType;
}

void TTpdu::SetControlDataType(telegram::TControlDataType type)
{
    CDataType = type;
}

std::vector<uint8_t> TTpdu::GetRaw() const
{
    uint8_t tpduFirstByte = 0;

    tpduFirstByte |= static_cast<uint8_t>(CommunicationType) << 6;
    tpduFirstByte |= (SequenceNumber & 0x0F) << 2;

    if ((CommunicationType == telegram::TCommunicationType::UDP) ||
        (CommunicationType == telegram::TCommunicationType::NDP))
    {
        std::vector<uint8_t> tpdu(ApduPayload.size() + 1, 0);
        tpdu[0] = tpduFirstByte;
        tpdu[0] |= (static_cast<uint8_t>(Apci) >> 2) & 0x03;
        tpdu[1] = ((static_cast<uint8_t>(Apci) & 0x03) << 6) | (ApduPayload[0] & 0x3F);
        std::copy(ApduPayload.begin() + 1, ApduPayload.end(), tpdu.begin() + 2);
        return tpdu;
    } else {
        std::vector<uint8_t> tpdu(1, 0);
        tpdu[0] = tpduFirstByte;
        tpdu[0] |= static_cast<uint8_t>(CDataType) & 0x03;
        return tpdu;
    }
}

void TTpdu::SetRaw(const std::vector<uint8_t>& tpduBuffer)
{
    SetRaw(tpduBuffer.cbegin(), tpduBuffer.cend());
}

void TTpdu::SetRaw(std::vector<uint8_t>::const_iterator beginIt, std::vector<uint8_t>::const_iterator endIt)
{
    SetCommunicationType(static_cast<telegram::TCommunicationType>(beginIt[0] >> 6));
    SetSequenceNumber((beginIt[0] >> 2) & 0x0F);

    if (CommunicationType == telegram::TCommunicationType::UDP ||
        CommunicationType == telegram::TCommunicationType::NDP) {
        SetAPCI(static_cast<telegram::TApci>(((beginIt[0] & 0x03) << 2) | (beginIt[1] >> 6)));
        SetPayload(beginIt + 1, endIt);
    } else {
        SetControlDataType(static_cast<telegram::TControlDataType>(beginIt[0] & 0x03));
        SetPayload({0});
    }
}

uint32_t TTpdu::Size() const
{
    return ApduPayload.size() + 1;
}

TTpdu::TTpdu(const std::vector<uint8_t>& tpduBuffer)
{
    SetRaw(tpduBuffer);
}
